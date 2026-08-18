#include "../include/isr.h"
#include "../include/pic.h"
#include "../include/stdio.h"
#include "../include/vga.h"
#include "../include/string.h"

static isr_t interrupt_handlers[256];

static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_init(void) {
    memset(interrupt_handlers, 0, sizeof(isr_t) * 256);
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void isr_handler(registers_t* regs) {
    if (interrupt_handlers[regs->int_no] != 0) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    } else {
        uint8_t old_color = vga_get_color();
        vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
        kprintf("\n [64-BIT KERNEL PANIC] CPU Exception %u: %s \n", (uint32_t)regs->int_no,
                regs->int_no < 32 ? exception_messages[regs->int_no] : "Unknown");
        kprintf(" RIP: 0x%p | CS: 0x%x | RFLAGS: 0x%p | ERR: 0x%x\n",
                (void*)regs->rip, (uint32_t)regs->cs, (void*)regs->rflags, (uint32_t)regs->err_code);
        vga_set_color(old_color);
        
        __asm__ volatile ("cli; hlt");
    }
}

void irq_handler(registers_t* regs) {
    if (interrupt_handlers[regs->int_no] != 0) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }

    pic_send_eoi((uint8_t)(regs->int_no - 32));
}
