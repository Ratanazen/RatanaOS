#include "../include/font.h"
#include "../include/font8x16.h"
#include "../include/gfx.h"
#include "../include/string.h"

static font_size_t active_font_size = FONT_SIZE_REGULAR;
static font_role_t active_font_role = FONT_ROLE_BODY;
static int active_font_scale_percent = 100;

static const char* font_size_names[FONT_SIZE_COUNT] = {
    "Small (12px)",
    "Regular (16px)",
    "Large (24px)",
    "Title (32px)"
};

void font_init(void) {
    active_font_size = FONT_SIZE_REGULAR;
    active_font_role = FONT_ROLE_BODY;
    active_font_scale_percent = 100;
}

void font_set_active_size(font_size_t size) {
    if (size >= 0 && size < FONT_SIZE_COUNT) {
        active_font_size = size;
    }
}

font_size_t font_get_active_size(void) {
    return active_font_size;
}

const char* font_get_size_name(font_size_t size) {
    if (size >= 0 && size < FONT_SIZE_COUNT) {
        return font_size_names[size];
    }
    return "Unknown";
}

void font_set(font_role_t role) {
    if (role < 0 || role >= FONT_ROLE_COUNT) return;
    active_font_role = role;
    active_font_size = font_get_size_for_role(role);
}

font_role_t font_get(void) {
    return active_font_role;
}

font_size_t font_get_size_for_role(font_role_t role) {
    switch (role) {
        case FONT_ROLE_CAPTION:
        case FONT_ROLE_SMALL:
            return FONT_SIZE_SMALL;
        case FONT_ROLE_BODY:
            return FONT_SIZE_REGULAR;
        case FONT_ROLE_BODY_LARGE:
        case FONT_ROLE_HEADING:
            return FONT_SIZE_LARGE;
        case FONT_ROLE_TITLE:
        case FONT_ROLE_DISPLAY:
            return FONT_SIZE_TITLE;
        default:
            return FONT_SIZE_REGULAR;
    }
}

void font_set_size(font_size_t size) {
    font_set_active_size(size);
}

font_size_t font_get_size(void) {
    return font_get_active_size();
}

bool font_set_size_px(int pixels) {
    if (pixels <= 0) return false;
    if (pixels <= 12) active_font_size = FONT_SIZE_SMALL;
    else if (pixels <= 16) active_font_size = FONT_SIZE_REGULAR;
    else if (pixels <= 24) active_font_size = FONT_SIZE_LARGE;
    else active_font_size = FONT_SIZE_TITLE;
    return true;
}

void font_set_scale(int percent) {
    if (percent < 80) percent = 80;
    if (percent > 150) percent = 150;
    active_font_scale_percent = percent;
}

int font_get_scale(void) {
    return active_font_scale_percent;
}

int font_scale_val(int base_size) {
    int scaled = (base_size * active_font_scale_percent + 50) / 100;
    return (scaled > 0) ? scaled : 1;
}

int font_get_char_width(font_size_t size) {
    switch (size) {
        case FONT_SIZE_SMALL:   return 6;
        case FONT_SIZE_REGULAR: return 8;
        case FONT_SIZE_LARGE:   return 12;
        case FONT_SIZE_TITLE:   return 16;
        default:                return 8;
    }
}

int font_get_char_height(font_size_t size) {
    switch (size) {
        case FONT_SIZE_SMALL:   return 12;
        case FONT_SIZE_REGULAR: return 16;
        case FONT_SIZE_LARGE:   return 24;
        case FONT_SIZE_TITLE:   return 32;
        default:                return 16;
    }
}

int font_measure_text_width(const char* str, font_size_t size) {
    if (!str) return 0;
    int len = (int)strlen(str);
    return len * font_get_char_width(size);
}

int font_measure_text_height(font_size_t size) {
    return font_get_char_height(size);
}

int font_measure_text(const char* str, font_size_t size) {
    return font_measure_text_width(str, size);
}

void font_draw_char(int x, int y, char c, uint32_t fg, uint32_t bg, font_size_t size) {
    if ((unsigned char)c >= 128) c = '?';
    const uint8_t* glyph = font8x16_basic[(unsigned char)c];

    if (size == FONT_SIZE_REGULAR) {
        for (int cy = 0; cy < 16; cy++) {
            uint8_t row = glyph[cy];
            for (int cx = 0; cx < 8; cx++) {
                if (row & (0x80 >> cx)) {
                    gfx_draw_pixel(x + cx, y + cy, fg);
                } else if (bg != 0) {
                    gfx_draw_pixel(x + cx, y + cy, bg);
                }
            }
        }
    } else if (size == FONT_SIZE_TITLE) {
        // 2x Scaled (16x32)
        for (int cy = 0; cy < 16; cy++) {
            uint8_t row = glyph[cy];
            for (int cx = 0; cx < 8; cx++) {
                bool on = (row & (0x80 >> cx)) != 0;
                uint32_t col = on ? fg : bg;
                if (on || bg != 0) {
                    gfx_draw_rect(x + cx * 2, y + cy * 2, 2, 2, col);
                }
            }
        }
    } else if (size == FONT_SIZE_LARGE) {
        // 1.5x Scaled (12x24)
        for (int cy = 0; cy < 24; cy++) {
            int src_y = (cy * 16) / 24;
            uint8_t row = glyph[src_y];
            for (int cx = 0; cx < 12; cx++) {
                int src_x = (cx * 8) / 12;
                if (row & (0x80 >> src_x)) {
                    gfx_draw_pixel(x + cx, y + cy, fg);
                } else if (bg != 0) {
                    gfx_draw_pixel(x + cx, y + cy, bg);
                }
            }
        }
    } else if (size == FONT_SIZE_SMALL) {
        // Compact 6x12
        for (int cy = 0; cy < 12; cy++) {
            int src_y = (cy * 16) / 12;
            uint8_t row = glyph[src_y];
            for (int cx = 0; cx < 6; cx++) {
                int src_x = (cx * 8) / 6;
                if (row & (0x80 >> src_x)) {
                    gfx_draw_pixel(x + cx, y + cy, fg);
                } else if (bg != 0) {
                    gfx_draw_pixel(x + cx, y + cy, bg);
                }
            }
        }
    }
}

void font_draw_text(int x, int y, const char* str, uint32_t fg, font_size_t size) {
    font_draw_text_bg(x, y, str, fg, 0, size);
}

void font_draw_text_bg(int x, int y, const char* str, uint32_t fg, uint32_t bg, font_size_t size) {
    if (!str) return;
    int cw = font_get_char_width(size);
    int ch = font_get_char_height(size);
    int cur_x = x;
    int cur_y = y;

    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            cur_y += ch;
        } else {
            font_draw_char(cur_x, cur_y, *str, fg, bg, size);
            cur_x += cw;
        }
        str++;
    }
}

void font_draw_text_center(int x, int y, int w, int h, const char* str, uint32_t fg, font_size_t size) {
    if (!str) return;
    int tw = font_measure_text_width(str, size);
    int th = font_measure_text_height(size);
    int tx = x + (w - tw) / 2;
    int ty = y + (h - th) / 2;
    font_draw_text(tx, ty, str, fg, size);
}

void font_draw_text_clipped(int x, int y, int max_w, const char* str, uint32_t fg, font_size_t size) {
    if (!str || max_w <= 0) return;
    int cw = font_get_char_width(size);
    int cur_x = x;

    while (*str) {
        if (cur_x + cw > x + max_w) break;
        font_draw_char(cur_x, y, *str, fg, 0, size);
        cur_x += cw;
        str++;
    }
}
