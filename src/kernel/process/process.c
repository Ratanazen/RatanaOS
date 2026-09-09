#include "../../include/process.h"
#include "../../include/physical.h"
#include "../../include/virtual.h"
#include "../../include/string.h"
#include "../../include/gdt.h"
#include "../../include/serial.h"

static process_t processes[MAX_PROCESSES];
process_t* current_process = NULL;
static uint64_t next_pid = 1;
static int current_proc_idx = 0;

void process_init(void) {
    memset(processes, 0, sizeof(processes));
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].state = PROCESS_DEAD;
    }
    serial_printf("PROCESS: Subsystem initialized\n");
}

#include "../../include/vfs.h"

process_t* process_create(void* entry_point, bool is_user) {
    int idx = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_DEAD) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return NULL;

    process_t* p = &processes[idx];
    memset(p, 0, sizeof(process_t));
    p->pid = next_pid++;
    p->state = PROCESS_READY;
    strcpy(p->cwd, "/");
    strcpy(p->name, "process");
    
    // Default standard I/O streams: stdin, stdout, stderr -> /dev/console
    vfs_node_t* console = vfs_open("/dev/console");
    p->fds[0] = console;
    p->fds[1] = console;
    p->fds[2] = console;
    
    // Create isolated address space
    p->pml4 = vmm_create_address_space();
    
    // Allocate 1 page for kernel stack (4KB)
    p->kernel_stack = (uint64_t)phys_alloc_page() + PAGE_SIZE; // Top of stack
    
    // Set up trap frame at the top of the kernel stack
    p->context = (trap_frame_t*)(p->kernel_stack - sizeof(trap_frame_t));
    memset(p->context, 0, sizeof(trap_frame_t));

    p->context->rip = (uint64_t)entry_point;
    p->context->rflags = 0x202; // IF enabled

    if (is_user) {
        p->context->cs = 0x18 | 3; // Ring 3 Code + RPL 3
        p->context->ss = 0x20 | 3; // Ring 3 Data + RPL 3
        
        // Allocate User Stack
        uint64_t user_stack_phys = (uint64_t)phys_alloc_page();
        uint64_t user_stack_virt = 0x00007FFFFFFFF000ULL; // High address
        vmm_map_page(p->pml4, user_stack_virt, user_stack_phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
        p->context->rsp = user_stack_virt + PAGE_SIZE;
    } else {
        p->context->cs = 0x08; // Ring 0 Code
        p->context->ss = 0x10; // Ring 0 Data
        p->context->rsp = p->kernel_stack;
    }

    return p;
}

trap_frame_t* process_switch(trap_frame_t* frame) {
    if (current_process != NULL && current_process->state == PROCESS_RUNNING) {
        current_process->context = frame; // Save context of current process
        current_process->state = PROCESS_READY;
    }

    int start_idx = (current_proc_idx + 1) % MAX_PROCESSES;
    int next_idx = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        int idx = (start_idx + i) % MAX_PROCESSES;
        if (processes[idx].state == PROCESS_READY) {
            next_idx = idx;
            break;
        }
    }

    if (next_idx == -1) {
        return frame; // No process to run
    }

    current_proc_idx = next_idx;
    current_process = &processes[next_idx];
    current_process->state = PROCESS_RUNNING;

    tss_set_kernel_stack(current_process->kernel_stack);
    vmm_switch_address_space(current_process->pml4);

    return current_process->context;
}

trap_frame_t* scheduler_tick(trap_frame_t* frame) {
    return process_switch(frame);
}

int process_fork(trap_frame_t* parent_context) {
    if (!current_process) return -1;
    
    int idx = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_DEAD) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return -1;
    
    process_t* parent = current_process;
    process_t* child = &processes[idx];
    
    memset(child, 0, sizeof(process_t));
    child->pid = next_pid++;
    child->parent_pid = parent->pid;
    child->state = PROCESS_READY;
    strcpy(child->name, parent->name);
    strcpy(child->cwd, parent->cwd);
    
    // Copy open file descriptors
    for (int i = 0; i < MAX_FDS; i++) {
        child->fds[i] = parent->fds[i];
    }
    
    // Clone Virtual Memory
    child->pml4 = vmm_clone_address_space(parent->pml4);
    if (!child->pml4) return -1;
    
    // Setup Kernel Stack
    child->kernel_stack = (uint64_t)phys_alloc_page() + PAGE_SIZE;
    
    // Copy the execution context (trap frame) to the top of the child's kernel stack
    child->context = (trap_frame_t*)(child->kernel_stack - sizeof(trap_frame_t));
    memcpy(child->context, parent_context, sizeof(trap_frame_t));
    
    // The child process returns 0 from the fork syscall
    child->context->rax = 0;
    
    return child->pid;
}

void process_exit(void) {
    if (current_process) {
        for (int i = 0; i < MAX_FDS; i++) {
            if (current_process->fds[i] && current_process->fds[i]->fs_type == FS_PIPE) {
                vfs_close(current_process->fds[i]);
            }
            current_process->fds[i] = NULL;
        }
        current_process->state = PROCESS_DEAD;
        __asm__ volatile("sti; 1: hlt; jmp 1b"); // Wait for next scheduler tick
    }
}
#include "../../include/process.h"
#include "../../include/timer.h" // For yielding

uint64_t sys_wait4(uint64_t pid, int* status) {
    if (!current_process) return -1;
    
    while (1) {
        bool found = false;
        bool all_dead = true;
        
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (processes[i].parent_pid == current_process->pid) {
                found = true;
                if (pid == (uint64_t)-1 || processes[i].pid == pid) {
                    if (processes[i].state == PROCESS_DEAD) {
                        if (status) *status = 0; // Fake status
                        uint64_t dead_pid = processes[i].pid;
                        // Reap process
                        processes[i].parent_pid = 0;
                        processes[i].pid = 0;
                        // In a real OS we'd free its memory here
                        return dead_pid;
                    }
                    all_dead = false;
                }
            }
        }
        
        if (!found) return (uint64_t)-1; // ECHILD
        
        // Yield if child is still running
        __asm__ volatile ("hlt");
    }
}
#include "../../include/process.h"

uint64_t sys_brk(uint64_t addr) {
    if (!current_process) return -1;
    
    if (addr == 0) {
        return current_process->brk_end;
    }
    
    // Simplistic brk implementation
    // Assuming heap starts at 0x800000000 (32GB)
    if (current_process->brk_start == 0) {
        current_process->brk_start = 0x800000000;
        current_process->brk_end = 0x800000000;
    }
    
    if (addr < current_process->brk_start) {
        return current_process->brk_end;
    }
    
    // Allocate pages if addr > brk_end
    uint64_t current_page = (current_process->brk_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    uint64_t new_page = (addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    
    while (current_page < new_page) {
        uint64_t phys = (uint64_t)phys_alloc_page();
        if (!phys) return current_process->brk_end; // OOM
        vmm_map_page(current_process->pml4, current_page, phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
        current_page += PAGE_SIZE;
    }
    
    current_process->brk_end = addr;
    return addr;
}
