#include "../../include/elf.h"
#include "../../include/vfs.h"
#include "../../include/syscall.h"
#include "../../include/serial.h"
#include "../../include/isr.h"
#include "../../include/process.h"
#include "../../include/string.h"
#include "../../include/heap.h"

static registers_t* syscall_handler_int80(registers_t* regs) {
    uint64_t syscall_num = regs->rax;
    
    switch (syscall_num) {
        case SYS_READ: { // sys_read(fd, buf, count)
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

        case SYS_WRITE: { // sys_write(fd, buf, count)
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

        case SYS_OPEN: { // sys_open(path, flags, mode)
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
            // Find free FD
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

        case SYS_CLOSE: { // sys_close(fd)
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

        case SYS_STAT: { // sys_stat(path, statbuf)
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
            st->st_ino = node->inode;
            st->st_mode = (node->flags == VFS_DIRECTORY) ? 0040755 : 0100644;
            regs->rax = 0;
            break;
        }

        case SYS_FSTAT: { // sys_fstat(fd, statbuf)
            int fd = (int)regs->rdi;
            stat_t* st = (stat_t*)regs->rsi;
            if (!st || !current_process || fd < 0 || fd >= MAX_FDS || !current_process->fds[fd]) {
                regs->rax = (uint64_t)-1;
                break;
            }
            vfs_node_t* node = current_process->fds[fd];
            memset(st, 0, sizeof(stat_t));
            st->st_size = node->size;
            st->st_ino = node->inode;
            st->st_mode = (node->flags == VFS_DIRECTORY) ? 0040755 : 0100644;
            regs->rax = 0;
            break;
        }

        case SYS_BRK: { // sys_brk(addr)
            uint64_t addr = regs->rdi;
            regs->rax = sys_brk(addr);
            break;
        }

        case SYS_PIPE: { // sys_pipe(pipefd[2])
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

        case SYS_YIELD: { // sys_yield()
            regs = (registers_t*)scheduler_tick((trap_frame_t*)regs);
            regs->rax = 0;
            break;
        }

        case SYS_DUP: { // sys_dup(oldfd)
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

        case SYS_DUP2: { // sys_dup2(oldfd, newfd)
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

        case SYS_GETPID: { // sys_getpid()
            regs->rax = current_process ? current_process->pid : 1;
            break;
        }

        case SYS_GETPPID: { // sys_getppid()
            regs->rax = current_process ? current_process->parent_pid : 0;
            break;
        }

        case SYS_FORK: { // sys_fork()
            regs->rax = process_fork((trap_frame_t*)regs);
            break;
        }

        case SYS_EXECVE: { // sys_execve(path, argv, envp)
            const char* path = (const char*)regs->rdi;
            vfs_node_t* file = vfs_open(path);
            if (file) {
                regs->rax = elf_execve(file, (trap_frame_t*)regs);
            } else {
                regs->rax = (uint64_t)-1;
            }
            break;
        }

        case SYS_EXIT: { // sys_exit(code)
            process_exit();
            break;
        }

        case SYS_WAIT4: { // sys_wait4(pid, status, options, rusage)
            uint64_t pid = regs->rdi;
            int* status = (int*)regs->rsi;
            regs->rax = sys_wait4(pid, status);
            break;
        }

        case SYS_UNAME: { // sys_uname(utsname)
            utsname_t* u = (utsname_t*)regs->rdi;
            if (!u) {
                regs->rax = (uint64_t)-1;
                break;
            }
            strcpy(u->sysname, "RatanaOS");
            strcpy(u->nodename, "ratana-macbook");
            strcpy(u->release, "15.4");
            strcpy(u->version, "1.0.0-macos-sequoia (x86_64 freestanding)");
            strcpy(u->machine, "x86_64");
            regs->rax = 0;
            break;
        }

        case SYS_GETCWD: { // sys_getcwd(buf, size)
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

        case SYS_CHDIR: { // sys_chdir(path)
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

        default:
            serial_printf("SYSCALL: Unsupported syscall %d\n", syscall_num);
            regs->rax = (uint64_t)-1;
            break;
    }
    
    return regs;
}

void syscall_init(void) {
    register_interrupt_handler(0x80, syscall_handler_int80);
    serial_printf("SYSCALL: Full RatanaOS ABI dispatcher registered on int 0x80\n");
}
