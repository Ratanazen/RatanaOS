#include "../include/mouse.h"
#include "../include/isr.h"
#include "../include/pic.h"
#include "../include/io.h"
#include "../include/gfx.h"

static uint8_t mouse_cycle = 0;
static int8_t  mouse_bytes[3];

static int mouse_x = 512;
static int mouse_y = 384;
static int max_bounds_x = 1024;
static int max_bounds_y = 768;

static bool left_button = false;
static bool right_button = false;

static void mouse_wait(uint8_t a_type) {
    uint32_t time_out = 100000;
    if (a_type == 0) {
        while (time_out--) {
            if ((inb(0x64) & 1) == 1) return;
        }
    } else {
        while (time_out--) {
            if ((inb(0x64) & 2) == 0) return;
        }
    }
}

static void mouse_write(uint8_t a_write) {
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, a_write);
}

static uint8_t mouse_read(void) {
    mouse_wait(0);
    return inb(0x60);
}

static void mouse_callback(registers_t* regs) {
    (void)regs;
    uint8_t status = inb(0x64);
    if (!(status & 0x20)) {
        return; // Data from keyboard, not mouse
    }

    uint8_t mouse_in = inb(0x60);

    switch (mouse_cycle) {
        case 0:
            if ((mouse_in & 0x08) == 0x08) { // Bit 3 must be 1 in first byte of PS/2 mouse packet
                mouse_bytes[0] = (int8_t)mouse_in;
                mouse_cycle++;
            }
            break;
        case 1:
            mouse_bytes[1] = (int8_t)mouse_in;
            mouse_cycle++;
            break;
        case 2:
            mouse_bytes[2] = (int8_t)mouse_in;
            mouse_cycle = 0;

            // Extract buttons
            left_button  = (mouse_bytes[0] & 0x01) != 0;
            right_button = (mouse_bytes[0] & 0x02) != 0;

            int rel_x = mouse_bytes[1];
            int rel_y = mouse_bytes[2];

            // Sign extension
            if (mouse_bytes[0] & 0x10) rel_x |= 0xFFFFFF00;
            if (mouse_bytes[0] & 0x20) rel_y |= 0xFFFFFF00;

            // Discard overflow packets
            if (mouse_bytes[0] & 0xC0) break;

            mouse_x += rel_x;
            mouse_y -= rel_y; // Invert Y delta for screen coordinates

            // Clamp to screen boundaries
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x >= max_bounds_x) mouse_x = max_bounds_x - 1;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y >= max_bounds_y) mouse_y = max_bounds_y - 1;
            break;
    }
}

void mouse_init(void) {
    mouse_cycle = 0;
    mouse_x = 512;
    mouse_y = 384;
    left_button = false;
    right_button = false;

    // Enable auxiliary mouse device
    mouse_wait(1);
    outb(0x64, 0xA8);

    // Enable interrupts
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    uint8_t status = (inb(0x60) | 2);
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);

    // Set defaults and enable data reporting
    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();

    // IRQ12 is vector 44
    register_interrupt_handler(IRQ12, mouse_callback);
}

int mouse_get_x(void) {
    return mouse_x;
}

int mouse_get_y(void) {
    return mouse_y;
}

bool mouse_is_left_clicked(void) {
    return left_button;
}

bool mouse_is_right_clicked(void) {
    return right_button;
}

void mouse_set_bounds(int max_x, int max_y) {
    max_bounds_x = max_x;
    max_bounds_y = max_y;
}

// 12x18 Arrow mouse cursor bitmap
static const char* cursor_bitmap[18] = {
    "X           ",
    "XX          ",
    "X.X         ",
    "X..X        ",
    "X...X       ",
    "X....X      ",
    "X.....X     ",
    "X......X    ",
    "X.......X   ",
    "X........X  ",
    "X.....XXXXX ",
    "X..X..X     ",
    "X.X X..X    ",
    "XX   X..X   ",
    "X     X..X  ",
    "      X..X  ",
    "       XX   ",
    "            "
};

void mouse_draw_cursor(int x, int y) {
    for (int row = 0; row < 18; row++) {
        for (int col = 0; col < 12; col++) {
            char p = cursor_bitmap[row][col];
            if (p == 'X') {
                gfx_draw_pixel(x + col, y + row, COLOR_BLACK);      // Black outline
            } else if (p == '.') {
                gfx_draw_pixel(x + col, y + row, COLOR_WHITE); // White fill
            }
        }
    }
}
