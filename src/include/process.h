#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"
#include "virtual.h"

#define MAX_PROCESSES 256
#define MAX_FDS 32

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_DEAD
} process_state_t;

struct trap_frame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed));
typedef struct trap_frame trap_frame_t;

struct vfs_node;

struct process {
    uint64_t pid;
    uint64_t parent_pid;
    char name[32];
    char cwd[64];
    uint64_t brk_start;
    uint64_t brk_end;
    process_state_t state;
    pml4_t* pml4;
    uint64_t kernel_stack;
    trap_frame_t* context;
    struct vfs_node* fds[MAX_FDS];
    int exit_code;
};
typedef struct process process_t;

void process_init(void);
process_t* process_create(void* entry_point, bool is_user);
int process_fork(trap_frame_t* parent_context);
trap_frame_t* process_switch(trap_frame_t* frame);
trap_frame_t* scheduler_tick(trap_frame_t* frame);
void process_exit(void);
extern process_t* current_process;

uint64_t sys_wait4(uint64_t pid, int* status);
uint64_t sys_brk(uint64_t addr);

#endif // PROCESS_H
