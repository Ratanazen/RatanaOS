#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

#define KEY_UP    0x80
#define KEY_DOWN  0x81
#define KEY_LEFT  0x82
#define KEY_RIGHT 0x83

void keyboard_init(void);
char keyboard_getchar(void);
bool keyboard_has_key(void);
bool keyboard_is_ctrl_pressed(void);
bool keyboard_is_alt_pressed(void);
bool keyboard_is_shift_pressed(void);

#endif // KEYBOARD_H
