#include "../include/stdio.h"
#include "../include/vga.h"
#include "../include/string.h"

void putchar(char c) {
    vga_putchar(c);
}

void puts(const char* str) {
    vga_writestring(str);
    vga_putchar('\n');
}

int kvprintf(const char* format, va_list args) {
    int printed = 0;
    char buffer[32];

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 'c': {
                    char c = (char)va_arg(args, int);
                    putchar(c);
                    printed++;
                    break;
                }
                case 's': {
                    const char* s = va_arg(args, const char*);
                    if (!s) s = "(null)";
                    vga_writestring(s);
                    printed += strlen(s);
                    break;
                }
                case 'd':
                case 'i': {
                    int d = va_arg(args, int);
                    itoa(d, buffer, 10);
                    vga_writestring(buffer);
                    printed += strlen(buffer);
                    break;
                }
                case 'u': {
                    uint32_t u = va_arg(args, uint32_t);
                    utoa(u, buffer, 10);
                    vga_writestring(buffer);
                    printed += strlen(buffer);
                    break;
                }
                case 'x': {
                    uint32_t x = va_arg(args, uint32_t);
                    utoa(x, buffer, 16);
                    vga_writestring(buffer);
                    printed += strlen(buffer);
                    break;
                }
                case 'X': {
                    uint32_t x = va_arg(args, uint32_t);
                    utoa(x, buffer, 16);
                    for (int j = 0; buffer[j]; j++) {
                        if (buffer[j] >= 'a' && buffer[j] <= 'f') {
                            buffer[j] = buffer[j] - 'a' + 'A';
                        }
                    }
                    vga_writestring(buffer);
                    printed += strlen(buffer);
                    break;
                }
                case 'p': {
                    uintptr_t p = (uintptr_t)va_arg(args, void*);
                    vga_writestring("0x");
                    utoa(p, buffer, 16);
                    vga_writestring(buffer);
                    printed += 2 + strlen(buffer);
                    break;
                }
                case '%': {
                    putchar('%');
                    printed++;
                    break;
                }
                default: {
                    putchar('%');
                    putchar(format[i]);
                    printed += 2;
                    break;
                }
            }
        } else {
            putchar(format[i]);
            printed++;
        }
    }
    return printed;
}

int kprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = kvprintf(format, args);
    va_end(args);
    return ret;
}
