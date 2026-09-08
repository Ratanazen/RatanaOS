#include "../include/ui_controls.h"
#include "../include/gfx.h"
#include "../include/string.h"

// -------------------------------------------------------------
// 1. Button Controls
// -------------------------------------------------------------
void ui_button_draw(int x, int y, int w, int h, const char* label, ui_button_style_t style, ui_control_state_t state) {
    const ui_theme_t* theme = theme_get_current();
    int r = theme->button_radius;

    uint32_t bg_col, border_col, txt_col;

    if (style == UI_BUTTON_PRIMARY) {
        bg_col = theme->accent;
        border_col = theme->accent;
        txt_col = theme->text_on_accent;

        if (state == UI_STATE_HOVER) {
            // Brighten slightly or add highlight
            bg_col = (bg_col == 0x000A84FF) ? 0x003399FF : bg_col;
        } else if (state == UI_STATE_PRESSED) {
            bg_col = (bg_col == 0x000A84FF) ? 0x000066CC : bg_col;
        }
    } else if (style == UI_BUTTON_DESTRUCTIVE) {
        bg_col = COLOR_RED;
        border_col = COLOR_RED;
        txt_col = COLOR_WHITE;
    } else { // Secondary
        bg_col = (state == UI_STATE_HOVER) ? theme->button_hover :
                 (state == UI_STATE_PRESSED) ? theme->button_pressed : theme->button_bg;
        border_col = theme->button_border;
        txt_col = theme->button_text;
    }

    if (state == UI_STATE_DISABLED) {
        txt_col = theme->text_disabled;
    }

    gfx_draw_rounded_rect(x, y, w, h, r, bg_col);
    gfx_draw_rounded_rect_outline(x, y, w, h, r, border_col);

    // Centered label
    font_draw_text_center(x, y, w, h, label, txt_col, FONT_SIZE_REGULAR);
}

bool ui_button_hit_test(int x, int y, int w, int h, int mx, int my) {
    return (mx >= x && mx < x + w && my >= y && my < y + h);
}

// -------------------------------------------------------------
// 2. Labels & Typography
// -------------------------------------------------------------
void ui_label_draw(int x, int y, const char* text, uint32_t color, font_size_t size) {
    font_draw_text(x, y, text, color, size);
}

void ui_label_draw_primary(int x, int y, const char* text) {
    const ui_theme_t* theme = theme_get_current();
    font_draw_text(x, y, text, theme->text_primary, FONT_SIZE_REGULAR);
}

void ui_label_draw_secondary(int x, int y, const char* text) {
    const ui_theme_t* theme = theme_get_current();
    font_draw_text(x, y, text, theme->text_secondary, FONT_SIZE_REGULAR);
}

void ui_label_draw_header(int x, int y, const char* text) {
    const ui_theme_t* theme = theme_get_current();
    font_draw_text(x, y, text, theme->text_primary, FONT_SIZE_LARGE);
}

// -------------------------------------------------------------
// 3. Sidebar Navigation
// -------------------------------------------------------------
void ui_sidebar_draw(int x, int y, int w, int h, const char* title, const char** items, int item_count, int active_idx, int hover_idx) {
    const ui_theme_t* theme = theme_get_current();

    gfx_draw_rect(x, y, w, h, theme->sidebar_bg);
    gfx_draw_line(x + w - 1, y, x + w - 1, y + h, theme->sidebar_border);

    if (title) {
        font_draw_text(x + 12, y + 10, title, theme->text_secondary, FONT_SIZE_SMALL);
    }

    int item_y = y + 30;
    int item_h = 24;
    int pad_x = 8;
    int inner_w = w - pad_x * 2;

    for (int i = 0; i < item_count; i++) {
        int iy = item_y + i * (item_h + 4);
        if (i == active_idx) {
            gfx_draw_rounded_rect(x + pad_x, iy, inner_w, item_h, theme->sidebar_radius, theme->sidebar_item_active);
            font_draw_text(x + pad_x + 10, iy + 4, items[i], theme->text_on_accent, FONT_SIZE_REGULAR);
        } else if (i == hover_idx) {
            gfx_draw_rounded_rect(x + pad_x, iy, inner_w, item_h, theme->sidebar_radius, theme->sidebar_item_hover);
            font_draw_text(x + pad_x + 10, iy + 4, items[i], theme->text_primary, FONT_SIZE_REGULAR);
        } else {
            font_draw_text(x + pad_x + 10, iy + 4, items[i], theme->text_primary, FONT_SIZE_REGULAR);
        }
    }
}

int ui_sidebar_hit_test(int x, int y, int w, int h, int item_count, int mx, int my) {
    (void)h;
    int item_y = y + 30;
    int item_h = 24;
    int pad_x = 8;
    int inner_w = w - pad_x * 2;

    if (mx < x + pad_x || mx >= x + pad_x + inner_w) return -1;

    for (int i = 0; i < item_count; i++) {
        int iy = item_y + i * (item_h + 4);
        if (my >= iy && my < iy + item_h) {
            return i;
        }
    }
    return -1;
}

// -------------------------------------------------------------
// 4. Checkbox
// -------------------------------------------------------------
void ui_checkbox_draw(int x, int y, const char* label, bool checked, bool hover) {
    const ui_theme_t* theme = theme_get_current();
    int box_s = 16;
    int r = 3;

    uint32_t bg = checked ? theme->accent : (hover ? theme->button_hover : theme->control_bg);
    uint32_t border = checked ? theme->accent : theme->control_border;

    gfx_draw_rounded_rect(x, y, box_s, box_s, r, bg);
    gfx_draw_rounded_rect_outline(x, y, box_s, box_s, r, border);

    if (checked) {
        // White checkmark lines
        gfx_draw_line(x + 4, y + 8, x + 7, y + 11, theme->text_on_accent);
        gfx_draw_line(x + 7, y + 11, x + 12, y + 4, theme->text_on_accent);
    }

    if (label) {
        font_draw_text(x + box_s + 8, y + 1, label, theme->text_primary, FONT_SIZE_REGULAR);
    }
}

bool ui_checkbox_hit_test(int x, int y, const char* label, int mx, int my) {
    int box_s = 16;
    int total_w = box_s + 8 + (label ? font_measure_text_width(label, FONT_SIZE_REGULAR) : 0);
    return (mx >= x && mx < x + total_w && my >= y && my < y + box_s);
}

// -------------------------------------------------------------
// 5. Toggle Switch (macOS Aqua Style)
// -------------------------------------------------------------
void ui_toggle_draw(int x, int y, bool on, bool hover) {
    const ui_theme_t* theme = theme_get_current();
    int tw = 38;
    int th = 20;
    int tr = 10;

    uint32_t track_col = on ? theme->control_active : (hover ? theme->button_hover : theme->control_bg);
    gfx_draw_rounded_rect(x, y, tw, th, tr, track_col);

    int knob_cx = on ? (x + tw - tr - 1) : (x + tr + 1);
    int knob_cy = y + th / 2;
    gfx_draw_circle(knob_cx, knob_cy, 7, COLOR_WHITE);
}

bool ui_toggle_hit_test(int x, int y, int mx, int my) {
    return (mx >= x && mx < x + 38 && my >= y && my < y + 20);
}

// -------------------------------------------------------------
// 6. Slider
// -------------------------------------------------------------
void ui_slider_draw(int x, int y, int w, int min_val, int max_val, int cur_val, bool hover) {
    const ui_theme_t* theme = theme_get_current();
    int track_h = 4;
    int ty = y + 8;

    if (cur_val < min_val) cur_val = min_val;
    if (cur_val > max_val) cur_val = max_val;
    int range = (max_val > min_val) ? (max_val - min_val) : 1;
    int fill_w = ((cur_val - min_val) * (w - 14)) / range;

    // Background track
    gfx_draw_rounded_rect(x, ty, w, track_h, 2, theme->control_bg);
    // Filled active track
    if (fill_w > 0) {
        gfx_draw_rounded_rect(x, ty, fill_w + 7, track_h, 2, theme->accent);
    }

    // Knob handle
    int kx = x + fill_w + 7;
    int ky = y + 10;
    gfx_draw_circle(kx, ky, 7, COLOR_WHITE);
    gfx_draw_circle(kx, ky, 8, hover ? theme->accent : theme->border);
}

bool ui_slider_hit_test(int x, int y, int w, int mx, int my) {
    return (mx >= x - 8 && mx <= x + w + 8 && my >= y && my <= y + 20);
}

int ui_slider_handle_drag(int x, int y, int w, int min_val, int max_val, int mx) {
    (void)y;
    int rel = mx - (x + 7);
    int track_usable = w - 14;
    if (track_usable <= 0) return min_val;

    if (rel < 0) rel = 0;
    if (rel > track_usable) rel = track_usable;

    int range = max_val - min_val;
    return min_val + (rel * range) / track_usable;
}

// -------------------------------------------------------------
// 7. Text Field
// -------------------------------------------------------------
void ui_textfield_draw(int x, int y, int w, int h, const char* text, const char* placeholder, bool focused) {
    const ui_theme_t* theme = theme_get_current();
    int r = 4;

    gfx_draw_rounded_rect(x, y, w, h, r, theme->control_bg);
    gfx_draw_rounded_rect_outline(x, y, w, h, r, focused ? theme->accent : theme->control_border);

    int pad_x = 8;
    int txt_y = y + (h - 16) / 2;

    if (text && text[0] != '\0') {
        font_draw_text_clipped(x + pad_x, txt_y, w - pad_x * 2, text, theme->text_primary, FONT_SIZE_REGULAR);
        if (focused) {
            int tw = font_measure_text_width(text, FONT_SIZE_REGULAR);
            if (tw < w - pad_x * 2 - 2) {
                gfx_draw_line(x + pad_x + tw + 1, txt_y, x + pad_x + tw + 1, txt_y + 14, theme->accent);
            }
        }
    } else if (placeholder) {
        font_draw_text_clipped(x + pad_x, txt_y, w - pad_x * 2, placeholder, theme->text_disabled, FONT_SIZE_REGULAR);
        if (focused) {
            gfx_draw_line(x + pad_x, txt_y, x + pad_x, txt_y + 14, theme->accent);
        }
    }
}

// -------------------------------------------------------------
// 8. Segmented Control / Button Group
// -------------------------------------------------------------
void ui_segmented_draw(int x, int y, int w, int h, const char** segments, int count, int active_idx, int hover_idx) {
    if (count <= 0) return;
    const ui_theme_t* theme = theme_get_current();
    int r = theme->button_radius;
    int seg_w = w / count;

    // Container background
    gfx_draw_rounded_rect(x, y, w, h, r, theme->control_bg);
    gfx_draw_rounded_rect_outline(x, y, w, h, r, theme->control_border);

    for (int i = 0; i < count; i++) {
        int sx = x + i * seg_w;
        int sw = (i == count - 1) ? (w - i * seg_w) : seg_w;

        if (i == active_idx) {
            gfx_draw_rounded_rect(sx + 2, y + 2, sw - 4, h - 4, r - 1, theme->accent);
            font_draw_text_center(sx, y, sw, h, segments[i], theme->text_on_accent, FONT_SIZE_REGULAR);
        } else if (i == hover_idx) {
            gfx_draw_rounded_rect(sx + 2, y + 2, sw - 4, h - 4, r - 1, theme->button_hover);
            font_draw_text_center(sx, y, sw, h, segments[i], theme->text_primary, FONT_SIZE_REGULAR);
        } else {
            font_draw_text_center(sx, y, sw, h, segments[i], theme->text_primary, FONT_SIZE_REGULAR);
        }

        if (i > 0 && i != active_idx && (i - 1) != active_idx) {
            gfx_draw_line(sx, y + 4, sx, y + h - 5, theme->separator);
        }
    }
}

int ui_segmented_hit_test(int x, int y, int w, int h, int count, int mx, int my) {
    if (count <= 0) return -1;
    if (mx < x || mx >= x + w || my < y || my >= y + h) return -1;

    int seg_w = w / count;
    int idx = (mx - x) / seg_w;
    if (idx >= count) idx = count - 1;
    return idx;
}

// -------------------------------------------------------------
// 9. Dropdown / Context Menu
// -------------------------------------------------------------
void ui_menu_draw(int x, int y, int w, const char** items, int item_count, int hover_idx) {
    const ui_theme_t* theme = theme_get_current();
    int item_h = 24;
    int pad = 6;
    int h = item_count * item_h + pad * 2;
    int r = 8;

    // Drop shadow
    gfx_draw_shadow(x, y, w, h, r, 6, 180);

    // Frosted Menu Container
    gfx_draw_rounded_rect_alpha(x, y, w, h, r, theme->panel_bg, theme->panel_alpha);
    gfx_draw_rounded_rect_outline(x, y, w, h, r, theme->panel_border);

    for (int i = 0; i < item_count; i++) {
        int iy = y + pad + i * item_h;
        if (strcmp(items[i], "---") == 0) {
            gfx_draw_line(x + 10, iy + item_h / 2, x + w - 10, iy + item_h / 2, theme->separator);
            continue;
        }

        if (i == hover_idx) {
            gfx_draw_rounded_rect(x + 4, iy, w - 8, item_h, 4, theme->accent);
            font_draw_text(x + 14, iy + 4, items[i], theme->text_on_accent, FONT_SIZE_REGULAR);
        } else {
            font_draw_text(x + 14, iy + 4, items[i], theme->text_primary, FONT_SIZE_REGULAR);
        }
    }
}

int ui_menu_hit_test(int x, int y, int w, int item_count, int mx, int my) {
    int item_h = 24;
    int pad = 6;
    int h = item_count * item_h + pad * 2;

    if (mx < x || mx >= x + w || my < y + pad || my >= y + h - pad) return -1;
    int idx = (my - (y + pad)) / item_h;
    if (idx >= 0 && idx < item_count) return idx;
    return -1;
}
