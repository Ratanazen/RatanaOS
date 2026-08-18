#ifndef SERIAL_H
#define SERIAL_H

#include "types.h"

#define COM1_PORT 0x3F8

void serial_init(void);
void serial_write_char(char c);
void serial_write_string(const char* str);
void serial_printf(const char* format, ...);

#endif // SERIAL_H
