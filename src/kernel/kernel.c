#include "../include/types.h"
#include "../include/vga.h"
#include "../include/stdio.h"
#include "../include/gdt.h"
#include "../include/idt.h"
#include "../include/isr.h"
#include "../include/pic.h"
#include "../include/timer.h"
#include "../include/keyboard.h"
#include "../include/pmm.h"
#include "../include/heap.h"
#include "../include/rtc.h"
#include "../include/serial.h"
#include "../include/speaker.h"
#include "../include/pci.h"
#include "../include/cpuid.h"
#include "../include/shell.h"

static void print_status(const char* message) {
    uint8_t old = vga_get_color();
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("[  OK  ] ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("%s\n", message);
    vga_set_color(old);

    serial_printf("[KERNEL OK] %s\n", message);
}

void kernel_main(uint32_t magic, uint32_t addr) {
    (void)magic;
    (void)addr;

    // 1. Initialize Serial COM1 port (early logging)
    serial_init();
    serial_printf("=== RatanaOS 2026 Initializing ===\n");

    // 2. Initialize VGA display driver
    vga_init();
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("================================================================================\n");
    kprintf("                Welcome to RatanaOS (2026 Full-Feature Edition)                 \n");
    kprintf("================================================================================\n\n");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    // 3. Initialize GDT (Global Descriptor Table)
    gdt_init();
    print_status("Global Descriptor Table (GDT) initialized");

    // 4. Initialize IDT (Interrupt Descriptor Table) & CPU ISRs
    idt_init();
    isr_init();
    print_status("Interrupt Descriptor Table (IDT & ISRs) loaded");

    // 5. Remap 8259 Dual PIC
    pic_remap(0x20, 0x28);
    print_status("Dual 8259 PIC remapped (IRQs 0-15 -> vectors 32-47)");

    // 6. Initialize PIT Timer (100 Hz)
    timer_init(100);
    print_status("Programmable Interval Timer (PIT) calibrated at 100 Hz");

    // 7. Initialize Physical Memory Manager (PMM) & Kernel Heap
    pmm_init(128 * 1024 * 1024); // 128MB RAM
    heap_init(0x00400000, 4 * 1024 * 1024); // 4MB Heap at 4MB-8MB
    print_status("PMM (128MB bitmap) & Kernel Heap (4MB) initialized");

    // 8. Initialize CMOS Real-Time Clock (RTC)
    rtc_init();
    print_status("CMOS Real-Time Clock (RTC) synchronized (2026)");

    // 9. Initialize CPUID & PCI Bus
    cpuid_init();
    pci_init();
    print_status("CPUID feature detector & PCI Bus scanner ready");

    // 10. Initialize PS/2 Keyboard Driver
    keyboard_init();
    print_status("PS/2 Keyboard driver initialized");

    // 11. Enable Hardware Interrupts
    __asm__ volatile ("sti");
    print_status("Hardware interrupts enabled (STI)");

    // Chirp PC Speaker on boot
    speaker_beep(1000, 40);

    kprintf("\nRatanaOS 2026 is ready! Type 'fetch', 'date', 'pci', or 'help'.\n\n");

    // 12. Launch Interactive Shell
    shell_init();

    // Main Kernel Idle & Event Loop
    while (1) {
        if (keyboard_has_key()) {
            char c = keyboard_getchar();
            shell_update(c);
        } else {
            __asm__ volatile ("hlt");
        }
    }
}
