[bits 64]
section .text
global idt_flush
extern isr_handler
extern irq_handler

idt_flush:
    lidt [rdi]
    ret

; Macros for 64-bit ISRs
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push qword 0        ; dummy error code
    push qword %1       ; interrupt number
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push qword %1       ; interrupt number (error code pushed by CPU)
    jmp isr_common_stub
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

isr_common_stub:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    call isr_handler
    mov rsp, rax
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16
    iretq

; Macros for 64-bit IRQs
%macro IRQ 2
global irq%1
irq%1:
    push qword 0
    push qword %2
    jmp irq_common_stub
%endmacro

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

irq_common_stub:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    call irq_handler
    mov rsp, rax
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16
    iretq

global isr128
isr128:
    push qword 0
    push qword 128
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov rdi, rsp
    extern isr_handler
    call isr_handler
    mov rsp, rax
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp, 16
    iretq

global syscall_entry_stub
extern syscall_handler_linux
syscall_entry_stub:
    swapgs
    mov [gs:0x00], rsp    ; Save user stack to gs_kernel_struct.user_rsp
    mov rsp, [gs:0x08]    ; Load kernel stack from gs_kernel_struct.kernel_rsp
    
    ; Construct trap_frame_t (168 bytes)
    ; We need to push 21 fields.
    ; SS, RSP, RFLAGS, CS, RIP are pushed by SYSCALL?!
    ; No! SYSCALL DOES NOT PUSH ANYTHING to the stack! It saves them in registers!
    ; RIP -> RCX
    ; RFLAGS -> R11
    ; CS -> STAR[47:32]
    ; SS -> STAR[47:32] + 8
    
    ; So we push them manually to emulate a trap_frame_t!
    push qword 0x23 ; SS (Ring 3 data)
    push qword [gs:0x00] ; RSP
    push r11 ; RFLAGS
    push qword 0x1B ; CS (Ring 3 code)
    push rcx ; RIP
    push qword 0 ; err_code
    push qword 0x80 ; int_no
    
    ; General registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    mov rdi, rsp   ; trap_frame_t* regs
    call syscall_handler_linux
    
    ; Restore general registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    add rsp, 16 ; skip int_no and err_code
    
    ; Pop RIP and RFLAGS back into RCX and R11
    pop rcx ; RIP
    add rsp, 8 ; skip CS
    pop r11 ; RFLAGS
    ; SS and RSP are restored via swapgs and SYSRET?
    ; Wait, SYSRET restores RIP from RCX, and RFLAGS from R11.
    ; RSP is restored from [gs:0x00] manually before SYSRET!
    
    mov rsp, [gs:0x00]    ; Restore user stack
    swapgs
    o64 sysret

