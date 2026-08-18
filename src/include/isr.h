#ifndef ISR_H
#define ISR_H

#include "types.h"

struct registers {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
};
typedef struct registers registers_t;

typedef void (*isr_t)(registers_t*);

void isr_init(void);
void register_interrupt_handler(uint8_t n, isr_t handler);
void isr_handler(registers_t* regs);
void irq_handler(registers_t* regs);

#endif // ISR_H
