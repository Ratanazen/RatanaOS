#ifndef STDIO_H
#define STDIO_H

#include "types.h"
#include <stdarg.h>

void putchar(char c);
void puts(const char* str);
int kprintf(const char* format, ...);
int kvprintf(const char* format, va_list args);
int ksprintf(char* str, const char* format, ...);
int ksnprintf(char* str, size_t size, const char* format, ...);
int printk(const char* format, ...);
void panic(const char* message, ...);
void klog(const char* level, const char* fmt, ...);

#endif // STDIO_H

