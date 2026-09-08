#ifndef UI_CONTROLS_H
#define UI_CONTROLS_H

#include "types.h"
#include "font.h"
#include "theme.h"

typedef enum {
    UI_STATE_NORMAL = 0,
    UI_STATE_HOVER,
    UI_STATE_PRESSED,
    UI_STATE_DISABLED
} ui_control_state_t;

typedef enum {
    UI_BUTTON_SECONDARY = 0,
    UI_BUTTON_PRIMARY,
    UI_BUTTON_DESTRUCTIVE
} ui_button_style_t;

// 1. Button Controls
void ui_button_draw(int x, int y, int w, int h, const char* label, ui_button_style_t style, ui_control_state_t state);
bool ui_button_hit_test(int x, int y, int w, int h, int mx, int my);

// 2. Labels & Typography
void ui_label_draw(int x, int y, const char* text, uint32_t color, font_size_t size);
void ui_label_draw_primary(int x, int y, const char* text);
void ui_label_draw_secondary(int x, int y, const char* text);
void ui_label_draw_header(int x, int y, const char* text);

// 3. Sidebar Navigation
void ui_sidebar_draw(int x, int y, int w, int h, const char* title, const char** items, int item_count, int active_idx, int hover_idx);
int  ui_sidebar_hit_test(int x, int y, int w, int h, int item_count, int mx, int my);

// 4. Checkbox
void ui_checkbox_draw(int x, int y, const char* label, bool checked, bool hover);
bool ui_checkbox_hit_test(int x, int y, const char* label, int mx, int my);

// 5. Toggle Switch (macOS Aqua Style)
void ui_toggle_draw(int x, int y, bool on, bool hover);
bool ui_toggle_hit_test(int x, int y, int mx, int my);

// 6. Slider
void ui_slider_draw(int x, int y, int w, int min_val, int max_val, int cur_val, bool hover);
int  ui_slider_handle_drag(int x, int y, int w, int min_val, int max_val, int mx);
bool ui_slider_hit_test(int x, int y, int w, int mx, int my);

// 7. Text Field
void ui_textfield_draw(int x, int y, int w, int h, const char* text, const char* placeholder, bool focused);

// 8. Segmented Control / Button Group
void ui_segmented_draw(int x, int y, int w, int h, const char** segments, int count, int active_idx, int hover_idx);
int  ui_segmented_hit_test(int x, int y, int w, int h, int count, int mx, int my);

// 9. Dropdown / Context Menu
void ui_menu_draw(int x, int y, int w, const char** items, int item_count, int hover_idx);
int  ui_menu_hit_test(int x, int y, int w, int item_count, int mx, int my);

#endif // UI_CONTROLS_H
