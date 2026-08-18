#include "../include/vga.h"
#include "../include/io.h"

static size_t vga_row;
static size_t vga_column;
static uint8_t vga_color_current;
static volatile uint16_t* vga_buffer = (volatile uint16_t*)0xB8000;

void vga_init(void) {
    vga_row = 0;
    vga_column = 0;
    vga_color_current = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
    vga_enable_cursor(14, 15);
}

void vga_set_color(uint8_t color) {
    vga_color_current = color;
}

uint8_t vga_get_color(void) {
    return vga_color_current;
}

void vga_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    vga_buffer[index] = vga_entry(c, color);
}

void vga_scroll(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', vga_color_current);
    }
}

void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color_current);
        }
    }
    vga_row = 0;
    vga_column = 0;
    vga_set_cursor(0, 0);
}

void vga_backspace(void) {
    if (vga_column > 0) {
        vga_column--;
        vga_putentryat(' ', vga_color_current, vga_column, vga_row);
        vga_set_cursor(vga_column, vga_row);
    } else if (vga_row > 0) {
        vga_row--;
        vga_column = VGA_WIDTH - 1;
        vga_putentryat(' ', vga_color_current, vga_column, vga_row);
        vga_set_cursor(vga_column, vga_row);
    }
}

void vga_putchar(char c) {
    if (c == '\n') {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_scroll();
            vga_row = VGA_HEIGHT - 1;
        }
    } else if (c == '\r') {
        vga_column = 0;
    } else if (c == '\t') {
        vga_column = (vga_column + 4) & ~(4 - 1);
        if (vga_column >= VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
                vga_row = VGA_HEIGHT - 1;
            }
        }
    } else if (c == '\b') {
        vga_backspace();
        return;
    } else {
        vga_putentryat(c, vga_color_current, vga_column, vga_row);
        if (++vga_column == VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
                vga_row = VGA_HEIGHT - 1;
            }
        }
    }
    vga_set_cursor(vga_column, vga_row);
}

void vga_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        vga_putchar(data[i]);
    }
}

void vga_writestring(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        vga_putchar(data[i]);
    }
}

void vga_set_cursor(size_t x, size_t y) {
    uint16_t pos = (uint16_t)(y * VGA_WIDTH + x);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void vga_disable_cursor(void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}
