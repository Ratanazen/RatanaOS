#include "../include/keyboard.h"
#include "../include/isr.h"
#include "../include/pic.h"
#include "../include/io.h"

#define KEYBOARD_BUFFER_SIZE 128

static char key_buffer[KEYBOARD_BUFFER_SIZE];
static int  buffer_head = 0;
static int  buffer_tail = 0;

static bool shift_pressed = false;
static bool caps_lock = false;

// Scan code table for US QWERTY keyboard (scancodes 0x00 to 0x58)
static const char scancodes_normal[128] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   /* Ctrl */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   /* Left Shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,   /* Right Shift */
    '*',
    0,   /* Alt */
    ' ', /* Space bar */
    0,   /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1 - F10 */
    0,   /* Num lock */
    0,   /* Scroll lock */
    0,   /* Home */
    0,   /* Up Arrow */
    0,   /* Page Up */
    '-',
    0,   /* Left Arrow */
    0,
    0,   /* Right Arrow */
    '+',
    0,   /* End */
    0,   /* Down Arrow */
    0,   /* Page Down */
    0,   /* Insert */
    0,   /* Delete */
    0, 0, 0,
    0,   /* F11 */
    0,   /* F12 */
    0    /* All other keys undefined */
};

static const char scancodes_shifted[128] = {
    0,   27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   /* Ctrl */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   /* Left Shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,   /* Right Shift */
    '*',
    0,   /* Alt */
    ' ', /* Space bar */
    0,   /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1 - F10 */
    0,   /* Num lock */
    0,   /* Scroll lock */
    0,   /* Home */
    0,   /* Up Arrow */
    0,   /* Page Up */
    '-',
    0,   /* Left Arrow */
    0,
    0,   /* Right Arrow */
    '+',
    0,   /* End */
    0,   /* Down Arrow */
    0,   /* Page Down */
    0,   /* Insert */
    0,   /* Delete */
    0, 0, 0,
    0,   /* F11 */
    0,   /* F12 */
    0
};

static void keyboard_callback(registers_t* regs) {
    (void)regs;
    uint8_t scancode = inb(0x60);

    if (scancode & 0x80) {
        uint8_t released_key = scancode & 0x7F;
        if (released_key == 0x2A || released_key == 0x36) {
            shift_pressed = false;
        }
    } else {
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = true;
            return;
        }
        if (scancode == 0x3A) {
            caps_lock = !caps_lock;
            return;
        }

        char c = 0;
        if (shift_pressed) {
            c = scancodes_shifted[scancode];
        } else {
            c = scancodes_normal[scancode];
            if (caps_lock && (c >= 'a' && c <= 'z')) {
                c = c - 'a' + 'A';
            }
        }

        if (c != 0) {
            int next_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
            if (next_head != buffer_tail) {
                key_buffer[buffer_head] = c;
                buffer_head = next_head;
            }
        }
    }
}

void keyboard_init(void) {
    buffer_head = 0;
    buffer_tail = 0;
    shift_pressed = false;
    caps_lock = false;
    register_interrupt_handler(IRQ1, keyboard_callback);
}

bool keyboard_has_key(void) {
    return buffer_head != buffer_tail;
}

char keyboard_getchar(void) {
    if (buffer_head == buffer_tail) {
        return 0;
    }
    char c = key_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}
