#include "../include/settings.h"
#include "../include/theme.h"
#include "../include/icons.h"
#include "../include/font.h"
#include "../include/dock.h"
#include "../include/stdio.h"
#include "../include/string.h"

static settings_t active_settings;
static settings_t saved_settings;

static uint32_t calculate_checksum(const settings_t* s) {
    const uint8_t* p = (const uint8_t*)s;
    size_t len = sizeof(settings_t) - sizeof(uint32_t);
    uint32_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum = (sum * 31) + p[i];
    }
    return sum;
}

void settings_reset_defaults(void) {
    active_settings.magic = SETTINGS_MAGIC;
    active_settings.version = 1;
    active_settings.ui_theme = UI_THEME_RATANA_DARK;
    active_settings.icon_theme = ICON_THEME_WHITESUR;
    active_settings.accent = ACCENT_BLUE;
    active_settings.ui_scale = 100;
    active_settings.font_size = FONT_SIZE_REGULAR;
    active_settings.dock_icon_size = 48;
    active_settings.dock_spacing = 6;
    active_settings.dock_magnification = true;
    active_settings.window_radius = 10;
    active_settings.transparency = true;
    active_settings.shadows = true;
    active_settings.checksum = calculate_checksum(&active_settings);
}

void settings_init(void) {
    settings_reset_defaults();
    saved_settings = active_settings;
    settings_apply();
}

settings_t* settings_get(void) {
    return &active_settings;
}

void settings_apply(void) {
    theme_set_preset(active_settings.ui_theme);
    theme_set_accent(active_settings.accent);
    theme_set_transparency(active_settings.transparency);
    theme_set_shadows(active_settings.shadows);
    theme_set_window_radius(active_settings.window_radius);
    ui_scale_set(active_settings.ui_scale);
    font_set_active_size(active_settings.font_size);
    icon_set_theme(active_settings.icon_theme);

    dock_set_icon_size(active_settings.dock_icon_size);
    dock_set_spacing(active_settings.dock_spacing);
    dock_set_magnification(active_settings.dock_magnification);
}

void settings_save(void) {
    active_settings.checksum = calculate_checksum(&active_settings);
    saved_settings = active_settings;
    kprintf("Settings saved to the volatile in-memory backend.\n");
}

void settings_load(void) {
    if (saved_settings.magic == SETTINGS_MAGIC &&
        saved_settings.checksum == calculate_checksum(&saved_settings)) {
        active_settings = saved_settings;
        settings_apply();
        kprintf("Settings loaded from the volatile in-memory backend.\n");
    } else {
        settings_reset_defaults();
        settings_apply();
    }
}

void settings_print(void) {
    kprintf("\n--- RatanaOS System Configuration ---\n");
    kprintf("UI Theme:           %s\n", theme_get_preset_name(active_settings.ui_theme));
    kprintf("Icon Theme:         %s\n", icon_get_theme_name());
    kprintf("Accent Color:       %s\n", theme_get_accent_name(active_settings.accent));
    kprintf("UI Scaling:         %d%%\n", active_settings.ui_scale);
    kprintf("Font Size:          %s\n", font_get_size_name(active_settings.font_size));
    kprintf("Dock Icon Size:     %d px\n", active_settings.dock_icon_size);
    kprintf("Dock Magnification: %s\n", active_settings.dock_magnification ? "Enabled" : "Disabled");
    kprintf("Window Radius:      %d px\n", active_settings.window_radius);
    kprintf("Transparency:       %s\n", active_settings.transparency ? "Enabled" : "Disabled");
    kprintf("Window Shadows:     %s\n", active_settings.shadows ? "Enabled" : "Disabled");
    kprintf("Storage Backend:    %s\n", settings_get_backend_name());
    kprintf("--------------------------------------\n\n");
}

settings_backend_t settings_get_backend(void) {
    return SETTINGS_BACKEND_VOLATILE;
}

const char* settings_get_backend_name(void) {
    return "Volatile memory (not persistent across reboot)";
}

bool settings_is_persistent(void) {
    return false;
}
