; RatanaOS 64-bit (x86_64 Long Mode) Bootloader & Multiboot Entry
[bits 32]

MBALIGN     equ  1 << 0             ; align loaded modules on page boundaries
MEMINFO     equ  1 << 1             ; provide memory map
FLAGS       equ  MBALIGN | MEMINFO  ; compatible with direct QEMU & GRUB
MAGIC       equ  0x1BADB002         ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)    ; checksum of above

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 4096
pml4_table:
    resb 4096
pdpt_table:
    resb 4096
pd_table:
    resb 4096

align 16
stack_bottom:
    resb 65536 ; 64 KiB 64-bit kernel stack
stack_top:

section .rodata
align 16
gdt64:
    dq 0 ; Null descriptor (0x00)
.code: equ $ - gdt64
    dq 0x00209A0000000000 ; 64-bit Code Segment (0x08)
.data: equ $ - gdt64
    dq 0x0000920000000000 ; 64-bit Data Segment (0x10)
gdt64_ptr:
    dw $ - gdt64 - 1
    dd gdt64

section .text
global _start
extern kernel_main

_start:
    cli
    cld

    ; Save Multiboot info pointer (EBX) and magic (EAX)
    mov edi, ebx
    mov esi, eax

    ; 1. Clear Page Tables
    mov ecx, 1024 * 3
    mov edx, pml4_table
.clear_tables:
    mov dword [edx], 0
    add edx, 4
    loop .clear_tables

    ; 2. Set up 4-Level Paging (Identity-map first 1GB with 2MB huge pages)
    ; Map PML4[0] -> PDPT
    mov eax, pdpt_table
    or eax, 0b11 ; present + writable
    mov [pml4_table], eax

    ; Map PDPT[0] -> PD
    mov eax, pd_table
    or eax, 0b11 ; present + writable
    mov [pdpt_table], eax

    ; Map 512 entries in PD (512 * 2MB = 1GB identity map)
    mov ecx, 0
.map_pd:
    mov eax, 0x200000 ; 2MB
    mul ecx           ; EAX = ecx * 2MB
    or eax, 0b10000011 ; present + writable + huge page (2MB)
    mov [pd_table + ecx * 8], eax
    mov dword [pd_table + ecx * 8 + 4], 0
    inc ecx
    cmp ecx, 512
    jne .map_pd

    ; 3. Load CR3 with PML4
    mov eax, pml4_table
    mov cr3, eax

    ; 4. Enable PAE in CR4 (bit 5) & OSFXSR (bit 9) & OSXMMEXCPT (bit 10)
    mov eax, cr4
    or eax, (1 << 5) | (1 << 9) | (1 << 10)
    mov cr4, eax

    ; 5. Set Long Mode Enable (LME) in EFER MSR (0xC0000080 bit 8)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 6. Enable Paging (bit 31), Protected Mode (bit 0), Monitor Coprocessor (bit 1), clear EM (bit 2)
    mov eax, cr0
    and eax, ~(1 << 2) ; Clear EM
    or eax, (1 << 31) | (1 << 0) | (1 << 1)
    mov cr0, eax

    ; 7. Load 64-bit GDT
    lgdt [gdt64_ptr]

    ; 8. Far jump into 64-bit code segment (0x08)
    jmp 0x08:long_mode_entry

[bits 64]
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Align 64-bit stack to 16 bytes
    mov rsp, stack_top
    and rsp, -16

    ; SysV ABI: RDI = 1st arg (multiboot info address), RSI = 2nd arg (magic)
    mov rdi, rdi
    mov rsi, rsi

    call kernel_main

    cli
.hang:
    hlt
    jmp .hang
