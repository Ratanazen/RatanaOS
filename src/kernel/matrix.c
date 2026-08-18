#include "../include/matrix.h"
#include "../include/vga.h"
#include "../include/timer.h"
#include "../include/keyboard.h"

#define MATRIX_COLS 80
#define MATRIX_ROWS 25

static uint32_t rand_state = 123456789;

static uint32_t rand_next(void) {
    rand_state = (rand_state * 1103515245 + 12345) & 0x7FFFFFFF;
    return rand_state;
}

void matrix_run(void) {
    uint8_t old_color = vga_get_color();
    vga_disable_cursor();
    vga_clear();

    int drops[MATRIX_COLS];
    for (int i = 0; i < MATRIX_COLS; i++) {
        drops[i] = -(int)(rand_next() % 25);
    }

    uint32_t frames = 0;
    while (frames < 300) { // Run for ~15 seconds or until key press
        if (keyboard_has_key()) {
            keyboard_getchar(); // Consume key
            break;
        }

        for (int x = 0; x < MATRIX_COLS; x++) {
            if (drops[x] >= 0 && drops[x] < MATRIX_ROWS) {
                // Character generation
                char c = (char)(33 + (rand_next() % 90));
                // Bright white head
                vga_putentryat(c, vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK), x, drops[x]);

                // Light green body
                if (drops[x] > 0) {
                    char trail1 = (char)(33 + (rand_next() % 90));
                    vga_putentryat(trail1, vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK), x, drops[x] - 1);
                }

                // Dark green tail
                if (drops[x] > 3) {
                    char trail2 = (char)(33 + (rand_next() % 90));
                    vga_putentryat(trail2, vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK), x, drops[x] - 3);
                }

                // Clear tail
                if (drops[x] > 8) {
                    vga_putentryat(' ', vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_BLACK), x, drops[x] - 8);
                }
            }

            drops[x]++;
            if (drops[x] - 8 >= MATRIX_ROWS) {
                drops[x] = -(int)(rand_next() % 15);
            }
        }

        timer_sleep_ms(40);
        frames++;
    }

    vga_set_color(old_color);
    vga_clear();
    vga_enable_cursor(14, 15);
}
