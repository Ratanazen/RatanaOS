; RatanaOS 64-bit (x86_64 Long Mode) Bootloader & Multiboot Entry
[bits 32]

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
    ; 64-bit Code Segment: Present, Ring 0, Executable, Read/Write, 64-bit Long Mode (L=1, D=0)
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; 0x00AF9A000000FFFF (0x08)
.data: equ $ - gdt64
    ; 64-bit Data Segment: Present, Ring 0, Read/Write
    dq (1 << 41) | (1 << 44) | (1 << 47)             ; 0x00AF92000000FFFF (0x10)
gdt64_ptr:
    dw $ - gdt64 - 1
    dq gdt64

section .text
global _start
extern kernel_main

_start:
    ; Set up initial 32-bit stack
    mov esp, stack_top

    ; Save Multiboot info pointer (EBX) and magic (EAX)
    mov edi, ebx
    mov esi, eax

    ; 1. Set up 4-Level Paging (Identity-map first 1GB with 2MB huge pages)
    ; Map PML4[0] -> PDPT
    mov eax, pdpt_table
    or eax, 0b11 ; present + writable
    mov [pml4_table], eax

    ; Map PDPT[0] -> PD
    mov eax, pd_table
    or eax, 0b11 ; present + writable
    mov [pdpt_table], eax

    ; Map 512 entries in PD_table (512 * 2MB = 1GB)
    mov ecx, 0
.map_pd_table:
    mov eax, 0x200000 ; 2MB
    mul ecx           ; EAX = ecx * 2MB
    or eax, 0b10000011 ; present + writable + huge page (2MB)
    mov [pd_table + ecx * 8], eax
    mov dword [pd_table + ecx * 8 + 4], 0
    inc ecx
    cmp ecx, 512
    jne .map_pd_table

    ; 2. Load CR3 with PML4 address
    mov eax, pml4_table
    mov cr3, eax

    ; 3. Enable PAE (Physical Address Extension) in CR4 (bit 5)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 4. Set Long Mode Enable (LME) bit in EFER MSR (0xC0000080 bit 8)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 5. Enable Paging & Protected Mode in CR0 (bit 31 & bit 0)
    mov eax, cr0
    or eax, (1 << 31) | (1 << 0)
    mov cr0, eax

    ; 6. Load 64-bit GDT
    lgdt [gdt64_ptr]

    ; 7. Far jump into 64-bit Long Mode code segment (0x08)
    jmp gdt64.code:long_mode_entry

[bits 64]
long_mode_entry:
    ; Reload data segment registers with 64-bit data segment selector (0x10)
    mov ax, gdt64.data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up native 64-bit RSP
    mov rsp, stack_top

    ; SysV x86_64 calling convention:
    ; RDI = 1st argument (Multiboot info address)
    ; RSI = 2nd argument (Multiboot magic)
    mov rdi, rdi
    mov rsi, rsi

    ; Call 64-bit C Kernel Main
    call kernel_main

    ; Halt if kernel returns
    cli
.hang:
    hlt
    jmp .hang
