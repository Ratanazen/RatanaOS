#ifndef IDT_H
#define IDT_H

#include "types.h"

struct idt_entry_struct {
    uint16_t base_low;   // Offset bits 0..15
    uint16_t sel;        // Code selector
    uint8_t  ist;        // Interrupt Stack Table
    uint8_t  flags;      // Type and attributes (0x8E)
    uint16_t base_mid;   // Offset bits 16..31
    uint32_t base_high;  // Offset bits 32..63
    uint32_t always0;    // Reserved
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

void idt_init(void);
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);

#endif // IDT_H
