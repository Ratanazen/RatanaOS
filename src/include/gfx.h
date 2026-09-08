#ifndef GFX_H
#define GFX_H

#include "types.h"
#include "multiboot.h"

// 32-bit ARGB Color definitions
#define COLOR_RGB(r, g, b) (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))
#define COLOR_RGBA(r, g, b, a) (((uint32_t)(a) << 24) | ((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

#define COLOR_BLACK       0x00000000
#define COLOR_WHITE       0x00FFFFFF
#define COLOR_RED         0x00FF453A
#define COLOR_GREEN       0x0030D158
#define COLOR_BLUE        0x000A84FF
#define COLOR_CYAN        0x005AC8FA
#define COLOR_YELLOW      0x00FFD60A
#define COLOR_ORANGE      0x00FF9F0A
#define COLOR_PURPLE      0x00BF5AF2
#define COLOR_GRAY        0x008E8E93
#define COLOR_DARK_GRAY   0x002C2C2E
#define COLOR_LIGHT_GRAY  0x00D1D1D6
#define COLOR_BORDER      0x003A3A3C
#define COLOR_PANEL_DARK  0x001C1C1E

// macOS Traffic Light Colors
#define COLOR_MAC_CLOSE   0x00FF5F56
#define COLOR_MAC_MIN     0x00FFBD2E
#define COLOR_MAC_ZOOM    0x0027C93F

typedef struct {
    uint32_t* frontbuffer;
    uint32_t* backbuffer;
    uint32_t* wallpaper_buffer;
    uint32_t  width;
    uint32_t  height;
    uint32_t  pitch;
    uint8_t   bpp;
    bool      active;
} gfx_context_t;

bool gfx_init(multiboot_info_t* mbi);
bool gfx_is_active(void);
int  gfx_get_width(void);
int  gfx_get_height(void);
uint32_t* gfx_get_backbuffer(void);

// Drawing primitives
void gfx_clear(uint32_t color);
void gfx_draw_pixel(int x, int y, uint32_t color);
void gfx_draw_pixel_alpha(int x, int y, uint32_t color, uint8_t alpha);
uint32_t gfx_get_pixel(int x, int y);

void gfx_draw_rect(int x, int y, int w, int h, uint32_t color);
void gfx_draw_rect_alpha(int x, int y, int w, int h, uint32_t color, uint8_t alpha);
void gfx_draw_rect_outline(int x, int y, int w, int h, uint32_t color);

void gfx_draw_rounded_rect(int x, int y, int w, int h, int r, uint32_t color);
void gfx_draw_rounded_rect_alpha(int x, int y, int w, int h, int r, uint32_t color, uint8_t alpha);
void gfx_draw_rounded_rect_outline(int x, int y, int w, int h, int r, uint32_t color);

void gfx_draw_circle(int cx, int cy, int radius, uint32_t color);
void gfx_draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void gfx_draw_gradient_v(int x, int y, int w, int h, uint32_t c1, uint32_t c2);

// Text & Wallpaper
void gfx_draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);
void gfx_draw_string(int x, int y, const char* str, uint32_t fg, uint32_t bg);
void gfx_draw_string_transparent(int x, int y, const char* str, uint32_t fg);

void gfx_init_wallpaper(void);
void gfx_draw_wallpaper(void);
void gfx_swap_buffers(void);

#endif // GFX_H
