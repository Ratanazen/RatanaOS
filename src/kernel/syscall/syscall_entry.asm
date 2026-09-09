[bits 64]
section .text
global syscall_entry
extern syscall_handler

syscall_entry:
    ; We are now in Ring 0, but RSP still points to the USER stack!
    ; We must switch to the kernel stack via swapgs to access per-CPU data, 
    ; or just use a statically allocated stack for now if it's single-core.
    ; Since we don't have per-CPU data yet, let's just save RSP to a scratch register 
    ; and load the kernel stack from a global variable (or the TSS, but reading TSS is hard in asm without a pointer).
    ; We will use a global `current_kernel_stack` for simplicity.
    
    ; Oh actually, we can just use swapgs and have a TSS pointer in GS, but let's use a global.
    
    ; Better yet, we can register an IDT interrupt 0x80 for now since it sets up the stack automatically!
    ; Let's switch back to int 0x80 to save time and guarantee stability for the First Boot Milestone.
