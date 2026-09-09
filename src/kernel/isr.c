#include "../include/serial.h"
#include "../include/isr.h"
#include "../include/pic.h"
#include "../include/stdio.h"
#include "../include/vga.h"
#include "../include/string.h"

static isr_t interrupt_handlers[256];

static const char* exception_messages[] = {
    "Division By Zero", "Debug", "Non Maskable Interrupt", "Breakpoint",
    "Into Detected Overflow", "Out of Bounds", "Invalid Opcode", "No Coprocessor",
    "Double Fault", "Coprocessor Segment Overrun", "Bad TSS", "Segment Not Present",
    "Stack Fault", "General Protection Fault", "Page Fault", "Unknown Interrupt",
    "Coprocessor Fault", "Alignment Check", "Machine Check", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved"
};

void isr_init(void) {
    memset(interrupt_handlers, 0, sizeof(isr_t) * 256);
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

uint64_t isr_handler(registers_t* regs) {
    if (interrupt_handlers[regs->int_no] != 0) {
        isr_t handler = interrupt_handlers[regs->int_no];
        regs = (registers_t*)handler(regs);
    } else {
        uint8_t old_color = vga_get_color();
        vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
        kprintf("\n [64-BIT KERNEL PANIC] CPU Exception %u: %s \n", (uint32_t)regs->int_no,
                regs->int_no < 32 ? exception_messages[regs->int_no] : "Unknown");
        kprintf(" RIP: 0x%p | CS: 0x%x | RFLAGS: 0x%p | ERR: 0x%x\n",
                (void*)regs->rip, (uint32_t)regs->cs, (void*)regs->rflags, (uint32_t)regs->err_code);
        vga_set_color(old_color);
        uint64_t cr2; __asm__ volatile("mov %%cr2, %0" : "=r"(cr2)); 
        char buf1[32], buf2[32]; 
utoa64(regs->rip, buf1, 16); 
utoa64(cr2, buf2, 16); 
serial_printf("\n [64-BIT KERNEL PANIC] CPU Exception %u (Error Code: %u) at RIP: 0x%s, CR2: 0x%s\n", (uint32_t)regs->int_no, (uint32_t)regs->err_code, buf1, buf2);
        
        __asm__ volatile ("cli; hlt");
    }
    return (uint64_t)regs;
}

uint64_t irq_handler(registers_t* regs) {
    serial_printf("IRQ: %d\n", regs->int_no);
    uint32_t irq = regs->int_no - 32;
    if (interrupt_handlers[regs->int_no] != 0) {
        isr_t handler = interrupt_handlers[regs->int_no];
        regs = (registers_t*)handler(regs);
    }

    pic_send_eoi(irq);
    return (uint64_t)regs;
}
