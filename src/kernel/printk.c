#include "../include/stdio.h"
#include <stdarg.h>

int printk(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = kvprintf(format, args);
    va_end(args);
    return ret;
}
