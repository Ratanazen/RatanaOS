#ifndef _STDIO_H
#define _STDIO_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#ifndef EOF
#define EOF (-1)
#endif

int putchar(int c);
int puts(const char* s);
int getchar(void);
int printf(const char* format, ...);
int sprintf(char* str, const char* format, ...);
int snprintf(char* str, size_t size, const char* format, ...);
int vsnprintf(char* str, size_t size, const char* format, va_list args);

#endif // _STDIO_H
