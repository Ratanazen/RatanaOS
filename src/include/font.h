#ifndef FONT_H
#define FONT_H

#include "types.h"

typedef enum {
    FONT_SIZE_SMALL = 0,   // ~6x12 / compact
    FONT_SIZE_REGULAR,     // 8x16 standard
    FONT_SIZE_LARGE,       // 12x24 / 1.5x
    FONT_SIZE_TITLE,       // 16x32 / 2.0x
    FONT_SIZE_COUNT
} font_size_t;

typedef enum {
    FONT_ROLE_SYSTEM = 0,
    FONT_ROLE_UI,
    FONT_ROLE_MENU,
    FONT_ROLE_TITLE,
    FONT_ROLE_CAPTION,
    FONT_ROLE_MONOSPACE,
    FONT_ROLE_COUNT
} font_role_t;

typedef enum {
    FONT_FAMILY_SYSTEM = 0,
    FONT_FAMILY_SAN_FRANCISCO,
    FONT_FAMILY_MONOSPACE,
    FONT_FAMILY_COUNT
} font_family_t;

void font_init(void);
void font_set_active_size(font_size_t size);
font_size_t font_get_active_size(void);
const char* font_get_size_name(font_size_t size);
void font_set(font_role_t role);
font_role_t font_get(void);
void font_set_size(font_size_t size);
font_size_t font_get_size(void);
bool font_set_size_px(int pixels);

int font_get_char_width(font_size_t size);
int font_get_char_height(font_size_t size);
int font_measure_text_width(const char* str, font_size_t size);
int font_measure_text_height(font_size_t size);
int font_measure_text(const char* str, font_size_t size);

void font_draw_char(int x, int y, char c, uint32_t fg, uint32_t bg, font_size_t size);
void font_draw_text(int x, int y, const char* str, uint32_t fg, font_size_t size);
void font_draw_text_bg(int x, int y, const char* str, uint32_t fg, uint32_t bg, font_size_t size);
void font_draw_text_center(int x, int y, int w, int h, const char* str, uint32_t fg, font_size_t size);
void font_draw_text_clipped(int x, int y, int max_w, const char* str, uint32_t fg, font_size_t size);

#endif // FONT_H
