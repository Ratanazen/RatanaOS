#include "../include/stdio.h"
#include "../include/vga.h"
#include "../include/string.h"

void putchar(char c) {
    vga_putchar(c);
}

void puts(const char* str) {
    if (str) {
        vga_writestring(str);
    }
    vga_putchar('\n');
}

static void print_padded(const char* str, int width, char pad_char, int* printed) {
    int len = (int)strlen(str);
    while (len < width) {
        putchar(pad_char);
        (*printed)++;
        width--;
    }
    vga_writestring(str);
    *printed += len;
}

int kvprintf(const char* format, va_list args) {
    if (!format) return 0;
    int printed = 0;
    char buffer[64];

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            char pad_char = ' ';
            int width = 0;

            if (format[i] == '0') {
                pad_char = '0';
                i++;
            }

            while (format[i] >= '0' && format[i] <= '9') {
                width = width * 10 + (format[i] - '0');
                i++;
            }

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
                    print_padded(s, width, pad_char, &printed);
                    break;
                }
                case 'd':
                case 'i': {
                    int d = va_arg(args, int);
                    itoa(d, buffer, 10);
                    print_padded(buffer, width, pad_char, &printed);
                    break;
                }
                case 'u': {
                    uint32_t u = va_arg(args, uint32_t);
                    utoa(u, buffer, 10);
                    print_padded(buffer, width, pad_char, &printed);
                    break;
                }
                case 'x': {
                    uint32_t x = va_arg(args, uint32_t);
                    utoa(x, buffer, 16);
                    print_padded(buffer, width, pad_char, &printed);
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
                    print_padded(buffer, width, pad_char, &printed);
                    break;
                }
                case 'p': {
                    uintptr_t p = (uintptr_t)va_arg(args, void*);
                    vga_writestring("0x");
                    printed += 2;
                    utoa(p, buffer, 16);
                    print_padded(buffer, width, pad_char, &printed);
                    break;
                }
                case '%': {
                    putchar('%');
                    printed++;
                    break;
                }
                default: {
                    putchar('%');
                    if (format[i] != '\0') {
                        putchar(format[i]);
                        printed += 2;
                    } else {
                        printed++;
                    }
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
