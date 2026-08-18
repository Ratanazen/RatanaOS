[bits 64]
section .text
global gdt_flush

gdt_flush:
    lgdt [rdi]
    ret
