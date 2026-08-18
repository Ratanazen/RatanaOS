#ifndef GFX_H
#define GFX_H

#include "types.h"
#include "multiboot.h"

// 32-bit ARGB Color definitions
#define COLOR_RGB(r, g, b) (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

#define COLOR_BLACK       0x00000000
#define COLOR_WHITE       0x00FFFFFF
#define COLOR_RED         0x00E74C3C
#define COLOR_GREEN       0x002ECC71
#define COLOR_BLUE        0x003498DB
#define COLOR_CYAN        0x001ABC9C
#define COLOR_DARK_BLUE   0x00172535
#define COLOR_ARCH_BLUE   0x001793D1
#define COLOR_BG_DARK     0x00181A20
#define COLOR_PANEL_DARK  0x00232731
#define COLOR_PANEL_LIGHT 0x002F3442
#define COLOR_BORDER      0x003E4452
#define COLOR_TEXT_WHITE  0x00F8F8F2
#define COLOR_TEXT_MUTED  0x008A93A5
#define COLOR_YELLOW      0x00F1C40F
#define COLOR_ORANGE      0x00E67E22
#define COLOR_GRAY        0x007F8C8D
#define COLOR_LIGHT_GRAY  0x00BDC3C7
#define COLOR_CLOSE_RED   0x00ED4245
#define COLOR_MIN_YELLOW  0x00FEE75C

typedef struct {
    uint32_t* frontbuffer;
    uint32_t* backbuffer;
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
void gfx_clear(uint32_t color);
void gfx_draw_pixel(int x, int y, uint32_t color);
void gfx_draw_rect(int x, int y, int w, int h, uint32_t color);
void gfx_draw_rect_outline(int x, int y, int w, int h, uint32_t color);
void gfx_draw_circle(int cx, int cy, int radius, uint32_t color);
void gfx_draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void gfx_draw_gradient_v(int x, int y, int w, int h, uint32_t c1, uint32_t c2);
void gfx_draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);
void gfx_draw_string(int x, int y, const char* str, uint32_t fg, uint32_t bg);
void gfx_draw_string_transparent(int x, int y, const char* str, uint32_t fg);
void gfx_swap_buffers(void);

#endif // GFX_H
