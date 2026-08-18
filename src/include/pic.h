#ifndef PIC_H
#define PIC_H

#include "types.h"

#define PIC1         0x20 // Master PIC base address
#define PIC2         0xA0 // Slave PIC base address
#define PIC1_COMMAND PIC1
#define PIC1_DATA    (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA    (PIC2 + 1)

#define PIC_EOI      0x20 // End of Interrupt command code

#define ICW1_INIT    0x10 // Initialization - required!
#define ICW1_ICW4    0x01 // ICW4 (not) needed
#define ICW4_8086    0x01 // 8086/88 (MCS-80/85) mode

#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

void pic_remap(int offset1, int offset2);
void pic_send_eoi(uint8_t irq);
void pic_set_mask(uint8_t irq_line);
void pic_clear_mask(uint8_t irq_line);

#endif // PIC_H
