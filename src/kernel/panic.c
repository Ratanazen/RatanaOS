#include "../include/stdio.h"
#include "../include/vga.h"
#include "../include/serial.h"
#include <stdarg.h>

void panic(const char* message, ...) {
    va_list args;
    uint8_t old_color = vga_get_color();
    
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
    kprintf("\n==================================================\n");
    kprintf("                KERNEL PANIC                       \n");
    kprintf("==================================================\n");
    
    va_start(args, message);
    kvprintf(message, args);
    va_end(args);
    
    kprintf("\n==================================================\n");
    vga_set_color(old_color);

    serial_printf("\n[KERNEL PANIC] ");
    va_start(args, message);
    // serial log
    char buf[512];
    ksnprintf(buf, sizeof(buf), message, args);
    serial_printf("%s\n", buf);
    va_end(args);

    __asm__ volatile ("cli; hlt");
    while (1) {}
}

void klog(const char* level, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[512];
    ksnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    kprintf("[%s] %s\n", level, buf);
    serial_printf("[%s] %s\n", level, buf);
}
