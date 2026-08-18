#include "../include/types.h"
#include "../include/multiboot.h"
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
#include "../include/gui.h"
#include "../include/shell.h"

static multiboot_info_t* global_mbi = NULL;

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
    global_mbi = (multiboot_info_t*)addr;

    // 1. Initialize Serial COM1 port (early logging)
    serial_init();
    serial_printf("=== RatanaOS 2026 GUI Edition Initializing ===\n");

    // 2. Initialize VGA display driver
    vga_init();
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("================================================================================\n");
    kprintf("              Welcome to RatanaOS (2026 Full GUI Desktop Edition)               \n");
    kprintf("================================================================================\n\n");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    // 3. Initialize GDT
    gdt_init();
    print_status("Global Descriptor Table (GDT) initialized");

    // 4. Initialize IDT & CPU ISRs
    idt_init();
    isr_init();
    print_status("Interrupt Descriptor Table (IDT & ISRs) loaded");

    // 5. Remap 8259 Dual PIC
    pic_remap(0x20, 0x28);
    print_status("Dual 8259 PIC remapped (IRQs 0-15 -> vectors 32-47)");

    // 6. Initialize PIT Timer (100 Hz)
    timer_init(100);
    print_status("Programmable Interval Timer (PIT) calibrated at 100 Hz");

    // 7. Initialize Physical Memory Manager (128MB) & Kernel Heap (8MB)
    pmm_init(128 * 1024 * 1024);
    heap_init(0x00400000, 8 * 1024 * 1024); // 8MB Heap for high-res GUI backbuffers
    print_status("PMM (128MB bitmap) & Kernel Heap (8MB) initialized");

    // 8. Initialize CMOS Real-Time Clock
    rtc_init();
    print_status("CMOS Real-Time Clock (RTC) synchronized (2026)");

    // 9. Initialize CPUID & PCI Bus
    cpuid_init();
    pci_init();
    print_status("CPUID feature detector & PCI Bus scanner ready");

    // 10. Initialize PS/2 Keyboard Driver
    keyboard_init();
    print_status("PS/2 Keyboard driver active");

    // 11. Initialize GUI Subsystem & Mouse Driver
    gui_init(global_mbi);
    print_status("VBE 32-bit Framebuffer & PS/2 Mouse Driver (IRQ12) initialized");

    // 12. Enable Hardware Interrupts
    __asm__ volatile ("sti");
    print_status("Hardware interrupts enabled (STI)");

    // Boot chime
    speaker_beep(1000, 30);
    speaker_beep(1500, 30);

    kprintf("\nRatanaOS is ready! Type 'gui' to launch Desktop, or 'help' for CLI.\n\n");

    // 13. Launch Interactive Shell
    shell_init();

    // Main Kernel Idle Loop
    while (1) {
        if (keyboard_has_key()) {
            char c = keyboard_getchar();
            shell_update(c);
        } else {
            __asm__ volatile ("hlt");
        }
    }
}
