[bits 64]
global _start
extern main
extern exit

section .text
_start:
    ; Align stack to 16 bytes
    mov rbp, 0
    push rbp
    mov rbp, rsp
    
    ; SysV calling convention: int main(int argc, char* argv[])
    xor rdi, rdi ; argc = 0
    xor rsi, rsi ; argv = NULL
    
    call main
    
    ; Pass return value from main to exit(status)
    mov rdi, rax
    call exit
    
    ; If exit returns, hang
.hang:
    mov rax, 60 ; SYS_EXIT
    mov rdi, 0
    int 0x80
    jmp .hang
