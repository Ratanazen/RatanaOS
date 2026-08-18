[bits 32]
section .text
global gdt_flush

gdt_flush:
    mov eax, [esp+4]  ; Pointer to gdt_ptr passed as argument
    lgdt [eax]

    mov ax, 0x10      ; Kernel Data Segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush   ; Far jump to Kernel Code Segment selector
.flush:
    ret
