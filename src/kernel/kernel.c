#include "../include/types.h"
#include "../include/vga.h"
#include "../include/stdio.h"
#include "../include/gdt.h"
#include "../include/idt.h"
#include "../include/isr.h"
#include "../include/pic.h"
#include "../include/timer.h"
#include "../include/keyboard.h"
#include "../include/shell.h"

static void print_status(const char* message) {
    uint8_t old = vga_get_color();
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("[  OK  ] ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("%s\n", message);
    vga_set_color(old);
}

void kernel_main(uint32_t magic, uint32_t addr) {
    (void)magic;
    (void)addr;

    // 1. Initialize VGA text display driver
    vga_init();
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("================================================================================\n");
    kprintf("                    Welcome to RatanaOS (32-bit x86 Kernel)                     \n");
    kprintf("================================================================================\n\n");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    // 2. Initialize GDT (Global Descriptor Table)
    gdt_init();
    print_status("Global Descriptor Table (GDT) initialized");

    // 3. Initialize IDT (Interrupt Descriptor Table) & CPU ISRs
    idt_init();
    isr_init();
    print_status("Interrupt Descriptor Table (IDT & ISRs) loaded");

    // 4. Remap 8259 Programmable Interrupt Controller (PIC)
    pic_remap(0x20, 0x28);
    print_status("Dual 8259 PIC remapped (IRQs 0-15 -> vectors 32-47)");

    // 5. Initialize PIT Timer (100 Hz)
    timer_init(100);
    print_status("Programmable Interval Timer (PIT) calibrated at 100 Hz");

    // 6. Initialize PS/2 Keyboard Driver
    keyboard_init();
    print_status("PS/2 Keyboard driver active");

    // 7. Enable hardware interrupts
    __asm__ volatile ("sti");
    print_status("Hardware interrupts enabled (STI)");

    kprintf("\nInitialization complete! Type 'fetch' or 'help' to begin.\n\n");

    // 8. Launch Interactive Shell
    shell_init();

    // Main Kernel Idle & Event Loop
    while (1) {
        if (keyboard_has_key()) {
            char c = keyboard_getchar();
            shell_update(c);
        } else {
            // Halt CPU until next interrupt arrives to save power
            __asm__ volatile ("hlt");
        }
    }
}
