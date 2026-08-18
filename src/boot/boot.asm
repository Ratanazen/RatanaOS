; Multiboot header and kernel entry point for RatanaOS (with Framebuffer Support)
MBALIGN     equ  1 << 0             ; align loaded modules on page boundaries
MEMINFO     equ  1 << 1             ; provide memory map
VIDINFO     equ  1 << 2             ; request video mode information
FLAGS       equ  MBALIGN | MEMINFO | VIDINFO
MAGIC       equ  0x1BADB002         ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)    ; checksum of above

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    ; Video mode request fields (since VIDINFO is set)
    dd 0    ; mode_type (0 = linear graphics framebuffer)
    dd 1024 ; width (1024 pixels)
    dd 768  ; height (768 pixels)
    dd 32   ; depth (32-bit color: ARGB)

section .bss
align 16
stack_bottom:
resb 32768 ; 32 KiB kernel stack
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

    ; Disable interrupts and halt CPU indefinitely if kernel_main returns
    cli
.hang:
    hlt
    jmp .hang
.end:
