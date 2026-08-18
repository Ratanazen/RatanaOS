#include "../include/snake.h"
#include "../include/vga.h"
#include "../include/timer.h"
#include "../include/keyboard.h"
#include "../include/speaker.h"
#include "../include/stdio.h"

#define MAX_SNAKE 256
#define BOARD_WIDTH 60
#define BOARD_HEIGHT 20

typedef struct {
    int x;
    int y;
} point_t;

static uint32_t seed = 987654321;
static uint32_t snake_rand(void) {
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return seed;
}

static bool is_on_snake(point_t pt, point_t* snake, int length) {
    for (int i = 0; i < length; i++) {
        if (snake[i].x == pt.x && snake[i].y == pt.y) {
            return true;
        }
    }
    return false;
}

void snake_game_start(void) {
    vga_disable_cursor();
    vga_clear();

    point_t snake[MAX_SNAKE];
    int length = 4;
    int dir_x = 1;
    int dir_y = 0;

    for (int i = 0; i < length; i++) {
        snake[i].x = 10 - i;
        snake[i].y = 10;
    }

    point_t food;
    food.x = 20;
    food.y = 10;

    int score = 0;
    bool game_over = false;

    // Draw frame
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    for (int x = 0; x < BOARD_WIDTH; x++) {
        vga_putentryat('#', vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK), x, 1);
        vga_putentryat('#', vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK), x, BOARD_HEIGHT + 2);
    }
    for (int y = 1; y <= BOARD_HEIGHT + 2; y++) {
        vga_putentryat('#', vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK), 0, y);
        vga_putentryat('#', vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK), BOARD_WIDTH - 1, y);
    }

    while (!game_over) {
        // Draw score header
        vga_set_cursor(2, 0);
        vga_set_color(vga_entry_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
        kprintf("RATANA SNAKE 2026 | Score: %d | Controls: WASD/Arrows | Quit: Q", score);

        // Process keyboard inputs
        while (keyboard_has_key()) {
            char key = keyboard_getchar();
            if (key == 'w' || key == 'W' || key == (char)KEY_UP) {
                if (dir_y == 0) { dir_x = 0; dir_y = -1; }
            } else if (key == 's' || key == 'S' || key == (char)KEY_DOWN) {
                if (dir_y == 0) { dir_x = 0; dir_y = 1; }
            } else if (key == 'a' || key == 'A' || key == (char)KEY_LEFT) {
                if (dir_x == 0) { dir_x = -1; dir_y = 0; }
            } else if (key == 'd' || key == 'D' || key == (char)KEY_RIGHT) {
                if (dir_x == 0) { dir_x = 1; dir_y = 0; }
            } else if (key == 'q' || key == 'Q') {
                game_over = true;
                break;
            }
        }

        if (game_over) break;

        // Calculate next head position
        point_t next_head;
        next_head.x = snake[0].x + dir_x;
        next_head.y = snake[0].y + dir_y;

        // Collision with walls
        if (next_head.x <= 0 || next_head.x >= BOARD_WIDTH - 1 ||
            next_head.y <= 1 || next_head.y >= BOARD_HEIGHT + 2) {
            game_over = true;
            break;
        }

        // Collision with self
        for (int i = 0; i < length; i++) {
            if (snake[i].x == next_head.x && snake[i].y == next_head.y) {
                game_over = true;
                break;
            }
        }
        if (game_over) break;

        // Clear tail on screen
        point_t tail = snake[length - 1];
        vga_putentryat(' ', vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_BLACK), tail.x, tail.y);

        // Move body
        for (int i = length - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }
        snake[0] = next_head;

        // Check if food eaten
        if (snake[0].x == food.x && snake[0].y == food.y) {
            score += 10;
            if (length < MAX_SNAKE - 1) {
                length++;
                snake[length - 1] = tail;
            }
            speaker_beep(880, 20); // Quick chirp

            // Spawn new food not on snake
            do {
                food.x = 2 + (snake_rand() % (BOARD_WIDTH - 4));
                food.y = 3 + (snake_rand() % (BOARD_HEIGHT - 2));
            } while (is_on_snake(food, snake, length));
        }

        // Draw food
        vga_putentryat('*', vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK), food.x, food.y);

        // Draw snake
        vga_putentryat('@', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK), snake[0].x, snake[0].y);
        for (int i = 1; i < length; i++) {
            vga_putentryat('o', vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK), snake[i].x, snake[i].y);
        }

        timer_sleep_ms(75);
    }

    speaker_beep(220, 200); // Game over tone
    vga_set_cursor(15, 10);
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
    kprintf(" GAME OVER! Final Score: %d ", score);
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    timer_sleep_ms(1500);

    vga_clear();
    vga_enable_cursor(14, 15);
}
