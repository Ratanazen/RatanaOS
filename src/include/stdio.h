#ifndef STDIO_H
#define STDIO_H

#include "types.h"
#include <stdarg.h>

void putchar(char c);
void puts(const char* str);
int kprintf(const char* format, ...);
int kvprintf(const char* format, va_list args);

#endif // STDIO_H
