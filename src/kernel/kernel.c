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

    serial_printf("[64-BIT KERNEL OK] %s\n", message);
}

void kernel_main(uint64_t mbi_addr, uint64_t magic) {
    (void)magic;
    global_mbi = (multiboot_info_t*)(uintptr_t)mbi_addr;

    serial_init();
    serial_printf("STEP 1: Serial initialized\n");

    vga_init();
    serial_printf("STEP 2: VGA initialized\n");

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("================================================================================\n");
    kprintf("            Welcome to RatanaOS 2026 (Native 64-bit x86_64 Long Mode)           \n");
    kprintf("================================================================================\n\n");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    gdt_init();
    serial_printf("STEP 3: GDT initialized\n");
    print_status("64-bit Long Mode Global Descriptor Table (GDT) initialized");

    idt_init();
    isr_init();
    serial_printf("STEP 4: IDT initialized\n");
    print_status("64-bit Interrupt Descriptor Table (IDT & ISRs, iretq) loaded");

    pic_remap(0x20, 0x28);
    serial_printf("STEP 5: PIC remapped\n");
    print_status("Dual 8259 PIC remapped (IRQs 0-15 -> vectors 32-47)");

    timer_init(100);
    serial_printf("STEP 6: Timer initialized\n");
    print_status("Programmable Interval Timer (PIT) calibrated at 100 Hz");

    pmm_init(256 * 1024 * 1024);
    heap_init(0x00400000, 16 * 1024 * 1024);
    serial_printf("STEP 7: PMM & Heap initialized\n");
    print_status("64-bit PMM (256MB bitmap) & Dynamic Heap (16MB) active");

    rtc_init();
    serial_printf("STEP 8: RTC initialized\n");
    print_status("CMOS Real-Time Clock (RTC) synchronized (2026)");

    cpuid_init();
    pci_init();
    serial_printf("STEP 9: CPUID & PCI initialized\n");
    print_status("64-bit CPUID feature detector & PCI Bus scanner ready");

    keyboard_init();
    serial_printf("STEP 10: Keyboard initialized\n");
    print_status("PS/2 Keyboard driver active");

    gui_init(global_mbi);
    serial_printf("STEP 11: GUI initialized\n");
    print_status("64-bit macOS Framebuffer & PS/2 Mouse Driver ready");

    __asm__ volatile ("sti");
    serial_printf("STEP 12: STI enabled\n");
    print_status("Hardware interrupts enabled in Long Mode (STI)");

    speaker_beep(1000, 30);
    speaker_beep(1500, 30);

    kprintf("\nRatanaOS 64-bit macOS Edition is ready! Type 'gui' or 'fetch'.\n\n");

    shell_init();
    serial_printf("STEP 13: Shell initialized\n");

    while (1) {
        if (keyboard_has_key()) {
            char c = keyboard_getchar();
            shell_update(c);
        } else {
            __asm__ volatile ("hlt");
        }
    }
}
