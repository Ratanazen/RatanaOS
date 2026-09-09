#include "../../include/elf.h"
#include "../../include/vfs.h"
#include "../../include/syscall.h"
#include "../../include/serial.h"
#include "../../include/isr.h"
#include "../../include/process.h"
#include "../../include/string.h"
#include "../../include/heap.h"
#include "../../include/timer.h"
#include "../../include/physical.h"
#include "../../include/virtual.h"

static uint64_t next_user_mmap_addr = 0x700000000000ULL;

static uint64_t do_mmap(uint64_t addr, size_t length, int prot, int flags, int fd, uint64_t offset) {
    (void)prot; (void)flags;
    if (!current_process || length == 0) return (uint64_t)-1;
    
    uint64_t vaddr = addr;
    if (vaddr == 0) {
        vaddr = next_user_mmap_addr;
        next_user_mmap_addr += (length + 0xFFF) & ~0xFFF;
    }
    
    uint64_t page_start = vaddr & ~0xFFF;
    uint64_t page_end = (vaddr + length + 0xFFF) & ~0xFFF;
    
    for (uint64_t a = page_start; a < page_end; a += PAGE_SIZE) {
        uint64_t phys = (uint64_t)phys_alloc_page();
        vmm_map_page(current_process->pml4, a, phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
    }
    
    uint64_t current_cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(current_cr3));
    vmm_switch_address_space(current_process->pml4);
    memset((void*)vaddr, 0, length);
    
    if (fd >= 0 && fd < MAX_FDS && current_process->fds[fd]) {
        vfs_node_t* node = current_process->fds[fd];
        vfs_read(node, (uint32_t)offset, (uint32_t)length, (uint8_t*)vaddr);
    }
    
    __asm__ volatile("mov %0, %%cr3" :: "r"(current_cr3));
    return vaddr;
}

static registers_t* do_syscall_dispatch(registers_t* regs) {
    uint64_t syscall_num = regs->rax;
    
    switch (syscall_num) {
        case SYS_READ: { // sys_read(fd, buf, count) - 0
            int fd = (int)regs->rdi;
            char* buf = (char*)regs->rsi;
            size_t count = (size_t)regs->rdx;
            if (!buf || count == 0) {
                regs->rax = 0;
                break;
            }
            if (!current_process || fd < 0 || fd >= MAX_FDS || !current_process->fds[fd]) {
                regs->rax = (uint64_t)-1;
                break;
            }
            vfs_node_t* node = current_process->fds[fd];
            int bytes = vfs_read(node, 0, (uint32_t)count, (uint8_t*)buf);
            regs->rax = (bytes >= 0) ? (uint64_t)bytes : (uint64_t)-1;
            break;
        }

        case SYS_WRITE: { // sys_write(fd, buf, count) - 1
            int fd = (int)regs->rdi;
            const char* buf = (const char*)regs->rsi;
            size_t count = (size_t)regs->rdx;
            if (!buf || count == 0) {
                regs->rax = 0;
                break;
            }
            if (fd == 1 || fd == 2) {
                for (size_t i = 0; i < count; i++) {
                    serial_printf("%c", buf[i]);
                }
                regs->rax = count;
                break;
            }
            if (!current_process || fd < 0 || fd >= MAX_FDS || !current_process->fds[fd]) {
                regs->rax = (uint64_t)-1;
                break;
            }
            vfs_node_t* node = current_process->fds[fd];
            int written = vfs_write(node, 0, (uint32_t)count, (const uint8_t*)buf);
            regs->rax = (written >= 0) ? (uint64_t)written : (uint64_t)-1;
            break;
        }

        case SYS_OPEN: { // sys_open(path, flags, mode) - 2
            const char* path = (const char*)regs->rdi;
            if (!path || !current_process) {
                regs->rax = (uint64_t)-1;
                break;
            }
            vfs_node_t* file = vfs_open(path);
            if (!file) {
                regs->rax = (uint64_t)-1;
                break;
            }
            int free_fd = -1;
            for (int i = 3; i < MAX_FDS; i++) {
                if (!current_process->fds[i]) {
                    free_fd = i;
                    break;
                }
            }
            if (free_fd == -1) {
                regs->rax = (uint64_t)-1;
                break;
            }
            current_process->fds[free_fd] = file;
            regs->rax = (uint64_t)free_fd;
            break;
        }

        case SYS_CLOSE: { // sys_close(fd) - 3
            int fd = (int)regs->rdi;
            if (!current_process || fd < 0 || fd >= MAX_FDS || !current_process->fds[fd]) {
                regs->rax = (uint64_t)-1;
                break;
            }
            vfs_close(current_process->fds[fd]);
            current_process->fds[fd] = NULL;
            regs->rax = 0;
            break;
        }

        case SYS_STAT: { // sys_stat(path, statbuf) - 4
            const char* path = (const char*)regs->rdi;
            stat_t* st = (stat_t*)regs->rsi;
            if (!path || !st) {
                regs->rax = (uint64_t)-1;
                break;
            }
            vfs_node_t* node = vfs_open(path);
            if (!node) {
                regs->rax = (uint64_t)-1;
                break;
            }
            memset(st, 0, sizeof(stat_t));
            st->st_size = node->size;
            st->st_ino = node->inode ? node->inode : 1;
            st->st_mode = (node->flags == VFS_DIRECTORY) ? 0040755 : 0100755;
            regs->rax = 0;
            break;
        }

        case SYS_FSTAT: { // sys_fstat(fd, statbuf) - 5
            int fd = (int)regs->rdi;
            stat_t* st = (stat_t*)regs->rsi;
            if (!st || !current_process || fd < 0 || fd >= MAX_FDS || !current_process->fds[fd]) {
                regs->rax = (uint64_t)-1;
                break;
            }
            vfs_node_t* node = current_process->fds[fd];
            memset(st, 0, sizeof(stat_t));
            st->st_size = node->size;
            st->st_ino = node->inode ? node->inode : 1;
            st->st_mode = (node->flags == VFS_DIRECTORY) ? 0040755 : 0100755;
            regs->rax = 0;
            break;
        }

        case SYS_LSEEK: { // sys_lseek(fd, offset, whence) - 8
            regs->rax = regs->rsi;
            break;
        }

        case SYS_MMAP: { // sys_mmap(addr, len, prot, flags, fd, off) - 9
            uint64_t addr = regs->rdi;
            size_t length = (size_t)regs->rsi;
            int prot = (int)regs->rdx;
            int flags = (int)regs->r10;
            int fd = (int)regs->r8;
            uint64_t offset = regs->r9;
            regs->rax = do_mmap(addr, length, prot, flags, fd, offset);
            break;
        }

        case 10: { // sys_mprotect(addr, len, prot)
            regs->rax = 0;
            break;
        }

        case SYS_MUNMAP: { // sys_munmap(addr, len) - 11
            regs->rax = 0;
            break;
        }

        case SYS_BRK: { // sys_brk(addr) - 12
            uint64_t addr = regs->rdi;
            regs->rax = sys_brk(addr);
            break;
        }

        case 13: { // Linux sys_rt_sigaction(sig, act, oact, sigsetsize)
            regs->rax = 0;
            break;
        }

        case 14: { // Linux sys_rt_sigprocmask(how, set, oset, sigsetsize)
            regs->rax = 0;
            break;
        }

        case SYS_IOCTL: { // sys_ioctl(fd, request, argp) - 16
            uint64_t req = regs->rsi;
            if (req == 0x5413) { // TIOCGWINSZ
                struct { uint16_t ws_row; uint16_t ws_col; uint16_t ws_xpixel; uint16_t ws_ypixel; } *ws = (void*)regs->rdx;
                if (ws) {
                    ws->ws_row = 24;
                    ws->ws_col = 80;
                    ws->ws_xpixel = 640;
                    ws->ws_ypixel = 480;
                }
            }
            regs->rax = 0;
            break;
        }

        case 21: { // Linux sys_access(pathname, mode)
            const char* path = (const char*)regs->rdi;
            vfs_node_t* node = vfs_open(path);
            regs->rax = node ? 0 : (uint64_t)-2; // 0 or -ENOENT
            break;
        }

        case SYS_PIPE: { // sys_pipe(pipefd[2]) - 22
            int* pipefd = (int*)regs->rdi;
            if (!pipefd || !current_process) {
                regs->rax = (uint64_t)-1;
                break;
            }
            vfs_node_t* r_node = NULL;
            vfs_node_t* w_node = NULL;
            if (vfs_create_pipe(&r_node, &w_node) < 0) {
                regs->rax = (uint64_t)-1;
                break;
            }
            int r_fd = -1, w_fd = -1;
            for (int i = 3; i < MAX_FDS; i++) {
                if (!current_process->fds[i]) {
                    if (r_fd == -1) r_fd = i;
                    else if (w_fd == -1) { w_fd = i; break; }
                }
            }
            if (r_fd == -1 || w_fd == -1) {
                vfs_close(r_node);
                vfs_close(w_node);
                regs->rax = (uint64_t)-1;
                break;
            }
            current_process->fds[r_fd] = r_node;
            current_process->fds[w_fd] = w_node;
            pipefd[0] = r_fd;
            pipefd[1] = w_fd;
            regs->rax = 0;
            break;
        }

        case SYS_YIELD: { // sys_yield() - 24
            regs = (registers_t*)scheduler_tick((trap_frame_t*)regs);
            regs->rax = 0;
            break;
        }

        case SYS_DUP: { // sys_dup(oldfd) - 32
            int oldfd = (int)regs->rdi;
            if (!current_process || oldfd < 0 || oldfd >= MAX_FDS || !current_process->fds[oldfd]) {
                regs->rax = (uint64_t)-1;
                break;
            }
            int newfd = -1;
            for (int i = 0; i < MAX_FDS; i++) {
                if (!current_process->fds[i]) {
                    newfd = i;
                    break;
                }
            }
            if (newfd == -1) {
                regs->rax = (uint64_t)-1;
                break;
            }
            current_process->fds[newfd] = current_process->fds[oldfd];
            regs->rax = (uint64_t)newfd;
            break;
        }

        case SYS_DUP2: { // sys_dup2(oldfd, newfd) - 33
            int oldfd = (int)regs->rdi;
            int newfd = (int)regs->rsi;
            if (!current_process || oldfd < 0 || oldfd >= MAX_FDS || newfd < 0 || newfd >= MAX_FDS || !current_process->fds[oldfd]) {
                regs->rax = (uint64_t)-1;
                break;
            }
            if (oldfd == newfd) {
                regs->rax = (uint64_t)newfd;
                break;
            }
            if (current_process->fds[newfd]) {
                vfs_close(current_process->fds[newfd]);
            }
            current_process->fds[newfd] = current_process->fds[oldfd];
            regs->rax = (uint64_t)newfd;
            break;
        }

        case SYS_GETPID: { // sys_getpid() - 39
            regs->rax = current_process ? current_process->pid : 1;
            break;
        }

        case 41: { // Linux sys_socket
            regs->rax = (uint64_t)-1; // ENOSYS
            break;
        }

        case 56:   // Linux sys_clone
        case SYS_FORK: { // sys_fork() - 57
            regs->rax = process_fork((trap_frame_t*)regs);
            break;
        }

        case SYS_EXECVE: { // sys_execve(path, argv, envp) - 59
            const char* path = (const char*)regs->rdi;
            vfs_node_t* file = vfs_open(path);
            if (file) {
                regs->rax = elf_execve(file, (trap_frame_t*)regs);
            } else {
                regs->rax = (uint64_t)-1;
            }
            break;
        }

        case SYS_EXIT: { // sys_exit(code) - 60
            process_exit();
            break;
        }

        case SYS_WAIT4: { // sys_wait4(pid, status, options, rusage) - 61
            uint64_t pid = regs->rdi;
            int* status = (int*)regs->rsi;
            regs->rax = sys_wait4(pid, status);
            break;
        }

        case SYS_UNAME: { // sys_uname(utsname) - 63
            utsname_t* u = (utsname_t*)regs->rdi;
            if (!u) {
                regs->rax = (uint64_t)-1;
                break;
            }
            strcpy(u->sysname, "Linux");
            strcpy(u->nodename, "ratanaos");
            strcpy(u->release, "6.6.0-ratana-x86_64");
            strcpy(u->version, "#1 SMP RatanaOS 2026");
            strcpy(u->machine, "x86_64");
            regs->rax = 0;
            break;
        }

        case 72: { // Linux sys_fcntl
            regs->rax = 0;
            break;
        }

        case SYS_GETCWD: { // sys_getcwd(buf, size) - 79
            char* buf = (char*)regs->rdi;
            size_t size = (size_t)regs->rsi;
            if (!buf || size == 0 || !current_process) {
                regs->rax = 0;
                break;
            }
            strncpy(buf, current_process->cwd, size);
            regs->rax = (uint64_t)buf;
            break;
        }

        case SYS_CHDIR: { // sys_chdir(path) - 80
            const char* path = (const char*)regs->rdi;
            if (!path || !current_process) {
                regs->rax = (uint64_t)-1;
                break;
            }
            strncpy(current_process->cwd, path, 63);
            current_process->cwd[63] = '\0';
            regs->rax = 0;
            break;
        }

        case SYS_MKDIR: { // sys_mkdir(path, mode) - 83
            regs->rax = 0;
            break;
        }

        case SYS_UNLINK: { // sys_unlink(path) - 87
            regs->rax = 0;
            break;
        }

        case 89: { // Linux sys_readlink(path, buf, bufsiz)
            const char* path = (const char*)regs->rdi;
            char* buf = (char*)regs->rsi;
            size_t bufsiz = (size_t)regs->rdx;
            if (path && buf && bufsiz > 0) {
                if (strcmp(path, "/proc/self/exe") == 0) {
                    strncpy(buf, current_process ? current_process->name : "/bin/hello", bufsiz);
                    regs->rax = strlen(buf);
                    break;
                }
            }
            regs->rax = (uint64_t)-2; // -ENOENT
            break;
        }

        case 96: { // Linux sys_gettimeofday(tv, tz)
            struct { uint64_t tv_sec; uint64_t tv_usec; } *tv = (void*)regs->rdi;
            if (tv) {
                tv->tv_sec = timer_get_ticks() / 100;
                tv->tv_usec = (timer_get_ticks() % 100) * 10000;
            }
            regs->rax = 0;
            break;
        }

        case 102: // getuid
        case 104: // getgid
        case 107: // geteuid
        case 108: // getegid
            regs->rax = 0; // Root identity
            break;

        case SYS_GETPPID: { // sys_getppid() - 110
            regs->rax = current_process ? current_process->parent_pid : 0;
            break;
        }

        case 158: { // Linux x86_64 sys_arch_prctl(code, addr)
            int code = (int)regs->rdi;
            uint64_t addr = regs->rsi;
            if (code == 0x1002) { // ARCH_SET_FS
                uint32_t low = (uint32_t)addr;
                uint32_t high = (uint32_t)(addr >> 32);
                __asm__ volatile ("wrmsr" : : "c"(0xC0000100), "a"(low), "d"(high));
                regs->rax = 0;
            } else if (code == 0x1001) { // ARCH_SET_GS
                uint32_t low = (uint32_t)addr;
                uint32_t high = (uint32_t)(addr >> 32);
                __asm__ volatile ("wrmsr" : : "c"(0xC0000101), "a"(low), "d"(high));
                regs->rax = 0;
            } else {
                regs->rax = (uint64_t)-1;
            }
            break;
        }

        case 218: { // Linux sys_set_tid_address(tidptr)
            regs->rax = current_process ? current_process->pid : 1;
            break;
        }

        case 228: { // Linux sys_clock_gettime(clockid, tp)
            struct { uint64_t tv_sec; uint64_t tv_nsec; } *tp = (void*)regs->rsi;
            if (tp) {
                tp->tv_sec = timer_get_ticks() / 100;
                tp->tv_nsec = (timer_get_ticks() % 100) * 10000000;
            }
            regs->rax = 0;
            break;
        }

        case 231: { // Linux x86_64 sys_exit_group(code)
            process_exit();
            break;
        }

        case 257: { // Linux sys_openat(dirfd, path, flags, mode)
            const char* path = (const char*)regs->rsi;
            if (!path || !current_process) {
                regs->rax = (uint64_t)-1;
                break;
            }
            vfs_node_t* file = vfs_open(path);
            if (!file) {
                regs->rax = (uint64_t)-1;
                break;
            }
            int free_fd = -1;
            for (int i = 3; i < MAX_FDS; i++) {
                if (!current_process->fds[i]) {
                    free_fd = i;
                    break;
                }
            }
            if (free_fd == -1) {
                regs->rax = (uint64_t)-1;
                break;
            }
            current_process->fds[free_fd] = file;
            regs->rax = (uint64_t)free_fd;
            break;
        }

        case 262: { // Linux sys_newfstatat(dirfd, path, statbuf, flags)
            const char* path = (const char*)regs->rsi;
            stat_t* st = (stat_t*)regs->rdx;
            if (!path || !st) {
                regs->rax = (uint64_t)-1;
                break;
            }
            vfs_node_t* node = vfs_open(path);
            if (!node) {
                regs->rax = (uint64_t)-1;
                break;
            }
            memset(st, 0, sizeof(stat_t));
            st->st_size = node->size;
            st->st_ino = node->inode ? node->inode : 1;
            st->st_mode = (node->flags == VFS_DIRECTORY) ? 0040755 : 0100755;
            regs->rax = 0;
            break;
        }

        case 302: { // Linux sys_prlimit64
            regs->rax = 0;
            break;
        }

        case 318: { // Linux sys_getrandom(buf, buflen, flags)
            char* buf = (char*)regs->rdi;
            size_t count = (size_t)regs->rsi;
            if (buf) {
                for (size_t i = 0; i < count; i++) buf[i] = (char)(i * 37 + 13);
            }
            regs->rax = count;
            break;
        }

        default:
            serial_printf("SYSCALL: Unsupported syscall %d\n", syscall_num);
            regs->rax = (uint64_t)-1;
            break;
    }
    
    return regs;
}

void syscall_handler_linux(registers_t* regs) {
    do_syscall_dispatch(regs);
}

static registers_t* syscall_handler_int80(registers_t* regs) {
    return do_syscall_dispatch(regs);
}


static uint64_t gs_kernel_struct[2]; // [0] = user_rsp, [1] = kernel_rsp
extern void syscall_entry_stub(void);
extern uint64_t current_kernel_stack;

void syscall_init(void) {
    register_interrupt_handler(0x80, syscall_handler_int80);
    serial_printf("SYSCALL: Full RatanaOS ABI dispatcher registered on int 0x80\n");
    
    // Setup x86_64 SYSCALL/SYSRET
    uint32_t eax, edx;
    
    // Enable SCE (bit 0) in EFER
    __asm__ volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(0xC0000080));
    eax |= 1;
    __asm__ volatile("wrmsr" :: "a"(eax), "d"(edx), "c"(0xC0000080));
    
    // STAR (0xC0000081): 
    // Bits 32-47: Kernel CS (0x08)
    // Bits 48-63: User CS (0x1B) - sysret uses this for CS and SS
    eax = 0;
    edx = (0x08 << 0) | (0x1B << 16); 
    __asm__ volatile("wrmsr" :: "a"(eax), "d"(edx), "c"(0xC0000081));
    
    // LSTAR (0xC0000082): RIP for syscall_entry_stub
    uint64_t lstar = (uint64_t)syscall_entry_stub;
    __asm__ volatile("wrmsr" :: "a"((uint32_t)lstar), "d"((uint32_t)(lstar >> 32)), "c"(0xC0000082));
    
    // FMASK (0xC0000084): Mask RFLAGS during syscall (disable IF)
    __asm__ volatile("wrmsr" :: "a"(0x200), "d"(0), "c"(0xC0000084));
    
    // Set KernelGSBase (0xC0000102) to our struct
    uint64_t gs_base = (uint64_t)&gs_kernel_struct;
    __asm__ volatile("wrmsr" :: "a"((uint32_t)gs_base), "d"((uint32_t)(gs_base >> 32)), "c"(0xC0000102));
}

void syscall_update_kernel_stack(uint64_t stack) {
    gs_kernel_struct[1] = stack;
}
