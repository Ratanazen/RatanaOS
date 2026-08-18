#ifndef ISR_H
#define ISR_H

#include "types.h"

struct registers {
    uint32_t ds;                                     // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    uint32_t int_no, err_code;                       // Interrupt number and error code
    uint32_t eip, cs, eflags, useresp, ss;           // Pushed by the processor automatically
};
typedef struct registers registers_t;

typedef void (*isr_t)(registers_t*);

void isr_init(void);
void register_interrupt_handler(uint8_t n, isr_t handler);
void isr_handler(registers_t* regs);
void irq_handler(registers_t* regs);

#endif // ISR_H
