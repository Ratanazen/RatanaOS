#include "../include/serial.h"
#include "../include/io.h"
#include "../include/string.h"
#include <stdarg.h>

void serial_init(void) {
    outb(COM1_PORT + 1, 0x00); // Disable all interrupts
    outb(COM1_PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(COM1_PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(COM1_PORT + 1, 0x00); //                  (hi byte)
    outb(COM1_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(COM1_PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

static int is_transmit_empty(void) {
    return inb(COM1_PORT + 5) & 0x20;
}

void serial_write_char(char c) {
    while (is_transmit_empty() == 0);
    outb(COM1_PORT, c);
}

void serial_write_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            serial_write_char('\r');
        }
        serial_write_char(str[i]);
    }
}

void serial_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buf[64];

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            char pad_char = ' ';
            int width = 0;
            int precision = -1;

            if (format[i] == '0') {
                pad_char = '0';
                i++;
            }

            while (format[i] >= '0' && format[i] <= '9') {
                width = width * 10 + (format[i] - '0');
                i++;
            }

            if (format[i] == '.') {
                i++;
                precision = 0;
                while (format[i] >= '0' && format[i] <= '9') {
                    precision = precision * 10 + (format[i] - '0');
                    i++;
                }
            }

            switch (format[i]) {
                case 's': {
                    const char* s = va_arg(args, const char*);
                    if (!s) s = "(null)";
                    int slen = (int)strlen(s);
                    if (precision >= 0 && precision < slen) slen = precision;
                    while (slen < width) {
                        serial_write_char(pad_char);
                        width--;
                    }
                    for (int j = 0; j < slen; j++) {
                        serial_write_char(s[j]);
                    }
                    break;
                }
                case 'd':
                case 'i': {
                    int d = va_arg(args, int);
                    itoa(d, buf, 10);
                    int blen = (int)strlen(buf);
                    while (blen < width) {
                        serial_write_char(pad_char);
                        width--;
                    }
                    serial_write_string(buf);
                    break;
                }
                case 'u': {
                    uint32_t u = va_arg(args, uint32_t);
                    utoa(u, buf, 10);
                    int blen = (int)strlen(buf);
                    while (blen < width) {
                        serial_write_char(pad_char);
                        width--;
                    }
                    serial_write_string(buf);
                    break;
                }
                case 'p':
                case 'x':
                case 'X': {
                    uint64_t x = (format[i] == 'p') ? va_arg(args, uint64_t) : va_arg(args, uint32_t);
                    utoa(x, buf, 16);
                    int blen = (int)strlen(buf);
                    while (blen < width) {
                        serial_write_char(pad_char);
                        width--;
                    }
                    serial_write_string(buf);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    serial_write_char(c);
                    break;
                }
                case '%': {
                    serial_write_char('%');
                    break;
                }
                default:
                    serial_write_char(format[i]);
                    break;
            }
        } else {
            if (format[i] == '\n') {
                serial_write_char('\r');
            }
            serial_write_char(format[i]);
        }
    }
    va_end(args);
}

