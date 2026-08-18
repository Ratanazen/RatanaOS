; Multiboot header and kernel entry point for RatanaOS
MBALIGN     equ  1 << 0             ; align loaded modules on page boundaries
MEMINFO     equ  1 << 1             ; provide memory map
FLAGS       equ  MBALIGN | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002         ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)    ; checksum of above, to prove we are multiboot

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB kernel stack
stack_top:

section .text
global _start:function (_start.end - _start)
extern kernel_main

_start:
    ; Set up the stack pointer
    mov esp, stack_top

    ; Reset EFLAGS
    push 0
    popf

    ; Push multiboot info struct pointer (EBX) and magic number (EAX)
    push ebx
    push eax

    ; Call high-level C kernel entry
    call kernel_main

    ; If kernel_main returns, disable interrupts and halt CPU indefinitely
    cli
.hang:
    hlt
    jmp .hang
.end:
