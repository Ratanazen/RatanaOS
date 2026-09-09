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
#include "../include/virtual.h"
#include "../include/process.h"
#include "../include/vfs.h"
#include "../include/elf.h"
#include "../include/syscall.h"
#include "../include/physical.h"
#include "../include/heap.h"
#include "../include/rtc.h"
#include "../include/serial.h"
#include "../include/speaker.h"
#include "../include/ata.h"
#include "../include/ext2.h"
#include "../include/pci.h"
#include "../include/cpuid.h"
#include "../include/acpi.h"
#include "../include/net.h"
#include "../include/gui.h"
#include "../include/shell.h"
#include "../include/packages.h"

static multiboot_info_t* global_mbi = NULL;

static void print_status(const char* message) {
    uint8_t old = vga_get_color();
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("%s\n", message);
    vga_set_color(old);

    serial_printf("[64-BIT KERNEL OK] %s\n", message);
}

void kernel_main(uint64_t mbi_addr, uint64_t magic) {
    (void)magic;
    global_mbi = (multiboot_info_t*)(uintptr_t)mbi_addr;

    serial_init();
    vga_init();
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    gdt_init();
    idt_init();
    isr_init();
    pic_remap(0x20, 0x28);
    timer_init(100);

    phys_init(global_mbi);
    vmm_init();
    heap_init(0x00400000, 16 * 1024 * 1024);

    process_init();
    vfs_init();
    ata_init();
    ext2_init();

    // Test VFS with Ext2
    vfs_node_t* hello_node = vfs_open("hello.txt");
    if (hello_node) {
        uint8_t file_buf[256];
        int bytes = vfs_read(hello_node, 0, 255, file_buf);
        if (bytes > 0) {
            file_buf[bytes] = '\0';
            serial_printf("VFS READ SUCCESS: %s\n", file_buf);
        }
    } else {
        serial_printf("VFS: Could not open hello.txt\n");
    }

    initramfs_init();
    syscall_init();
    
    // Load /sbin/init
    vfs_node_t* init_file = vfs_open("/sbin/init");
    if (init_file) {
        elf_load_and_run(init_file, "init");
        serial_printf("STEP 12: STI enabled\n");
        __asm__ volatile("sti");
    } else {
        serial_printf("ERROR: /sbin/init not found\n");
    }

    rtc_init();
    cpuid_init();
    pci_init();
    acpi_init();
    net_init();
    packages_init();
    keyboard_init();
    gui_init(global_mbi);

    speaker_beep(1000, 30);
    speaker_beep(1500, 30);
    gui_start();
    shell_init();

    while (1) {
        if (keyboard_has_key()) {
            char c = keyboard_getchar();
            shell_update(c);
        } else {
        }
    }
}
