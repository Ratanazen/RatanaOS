#include "../include/settings.h"
#include "../include/theme.h"
#include "../include/icons.h"
#include "../include/font.h"
#include "../include/dock.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/vfs.h"

#define SETTINGS_CONF_PATH "/etc/ratana/settings.conf"

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
    active_settings.show_desktop_icons = true;
    active_settings.desktop_icon_size = 48;
    active_settings.show_icon_labels = true;
    active_settings.window_radius = 10;
    active_settings.transparency = true;
    active_settings.shadows = true;

    active_settings.theme_mode = THEME_MODE_DARK;
    active_settings.window_style = WINDOW_STYLE_MACOS;
    active_settings.dock_position = DOCK_POS_BOTTOM;
    active_settings.dock_style = DOCK_STYLE_GLASS;
    active_settings.dock_autohide = false;
    active_settings.menubar_style = MENUBAR_STYLE_GLASS;
    active_settings.font_scale = 100;
    active_settings.window_shadow_size = 6;

    active_settings.checksum = calculate_checksum(&active_settings);
}

void settings_init(void) {
    settings_reset_defaults();
    saved_settings = active_settings;
    settings_load();
}

settings_t* settings_get(void) {
    return &active_settings;
}

void settings_apply(void) {
    theme_set_preset(active_settings.ui_theme);
    theme_set_mode(active_settings.theme_mode);
    theme_set_accent(active_settings.accent);
    theme_set_transparency(active_settings.transparency);
    theme_set_shadows(active_settings.shadows);
    theme_set_window_radius(active_settings.window_radius);
    theme_set_window_shadow_size(active_settings.window_shadow_size);
    theme_set_window_style(active_settings.window_style);
    theme_set_dock_position(active_settings.dock_position);
    theme_set_dock_style(active_settings.dock_style);
    theme_set_dock_autohide(active_settings.dock_autohide);
    theme_set_menubar_style(active_settings.menubar_style);
    theme_set_font_scale(active_settings.font_scale);
    ui_scale_set(active_settings.ui_scale);
    font_set_active_size(active_settings.font_size);

    dock_set_position(active_settings.dock_position);
    dock_set_style(active_settings.dock_style);
    dock_set_autohide(active_settings.dock_autohide);

    icon_config_t icfg;
    icfg.theme = active_settings.icon_theme;
    icfg.dock_icon_size = active_settings.dock_icon_size;
    icfg.dock_spacing = active_settings.dock_spacing;
    icfg.dock_magnification = active_settings.dock_magnification;
    icfg.show_desktop_icons = active_settings.show_desktop_icons;
    icfg.desktop_icon_size = active_settings.desktop_icon_size;
    icfg.show_icon_labels = active_settings.show_icon_labels;
    icon_config_set(&icfg);
}

void settings_save(void) {
    active_settings.checksum = calculate_checksum(&active_settings);
    saved_settings = active_settings;

    char conf_buf[1024];
    ksprintf(conf_buf,
             "ui_theme=%d\n"
             "icon_theme=%d\n"
             "accent=%d\n"
             "ui_scale=%d\n"
             "font_size=%d\n"
             "dock_icon_size=%d\n"
             "dock_spacing=%d\n"
             "dock_magnification=%d\n"
             "show_desktop_icons=%d\n"
             "desktop_icon_size=%d\n"
             "show_icon_labels=%d\n"
             "window_radius=%d\n"
             "transparency=%d\n"
             "shadows=%d\n"
             "theme_mode=%d\n"
             "window_style=%d\n"
             "dock_position=%d\n"
             "dock_style=%d\n"
             "dock_autohide=%d\n"
             "menubar_style=%d\n"
             "font_scale=%d\n"
             "window_shadow_size=%d\n",
             active_settings.ui_theme,
             active_settings.icon_theme,
             active_settings.accent,
             active_settings.ui_scale,
             active_settings.font_size,
             active_settings.dock_icon_size,
             active_settings.dock_spacing,
             active_settings.dock_magnification ? 1 : 0,
             active_settings.show_desktop_icons ? 1 : 0,
             active_settings.desktop_icon_size,
             active_settings.show_icon_labels ? 1 : 0,
             active_settings.window_radius,
             active_settings.transparency ? 1 : 0,
             active_settings.shadows ? 1 : 0,
             active_settings.theme_mode,
             active_settings.window_style,
             active_settings.dock_position,
             active_settings.dock_style,
             active_settings.dock_autohide ? 1 : 0,
             active_settings.menubar_style,
             active_settings.font_scale,
             active_settings.window_shadow_size);

    vfs_node_t* conf_node = vfs_create_file(SETTINGS_CONF_PATH, VFS_FILE);
    if (conf_node) {
        vfs_write(conf_node, 0, strlen(conf_buf), (const uint8_t*)conf_buf);
        kprintf("Settings saved to persistent config file: %s\n", SETTINGS_CONF_PATH);
    } else {
        kprintf("Settings saved to volatile in-memory backup.\n");
    }
}

void settings_load(void) {
    vfs_node_t* conf_node = vfs_open(SETTINGS_CONF_PATH);
    if (conf_node && conf_node->size > 0) {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int read_bytes = vfs_read(conf_node, 0, sizeof(buf) - 1, (uint8_t*)buf);
        if (read_bytes > 0) {
            buf[read_bytes] = '\0';
            // Parse line by line simple key=val
            char* line = buf;
            while (*line) {
                char* next_line = strchr(line, '\n');
                if (next_line) *next_line = '\0';

                char* eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    char* key = line;
                    char* val = eq + 1;
                    int val_i = atoi(val);

                    if (strcmp(key, "ui_theme") == 0) active_settings.ui_theme = (ui_theme_preset_t)val_i;
                    else if (strcmp(key, "icon_theme") == 0) active_settings.icon_theme = (icon_theme_id_t)val_i;
                    else if (strcmp(key, "accent") == 0) active_settings.accent = (ui_accent_color_t)val_i;
                    else if (strcmp(key, "ui_scale") == 0) active_settings.ui_scale = val_i;
                    else if (strcmp(key, "font_size") == 0) active_settings.font_size = (font_size_t)val_i;
                    else if (strcmp(key, "dock_icon_size") == 0) active_settings.dock_icon_size = val_i;
                    else if (strcmp(key, "dock_spacing") == 0) active_settings.dock_spacing = val_i;
                    else if (strcmp(key, "dock_magnification") == 0) active_settings.dock_magnification = (val_i != 0);
                    else if (strcmp(key, "show_desktop_icons") == 0) active_settings.show_desktop_icons = (val_i != 0);
                    else if (strcmp(key, "desktop_icon_size") == 0) active_settings.desktop_icon_size = val_i;
                    else if (strcmp(key, "show_icon_labels") == 0) active_settings.show_icon_labels = (val_i != 0);
                    else if (strcmp(key, "window_radius") == 0) active_settings.window_radius = val_i;
                    else if (strcmp(key, "transparency") == 0) active_settings.transparency = (val_i != 0);
                    else if (strcmp(key, "shadows") == 0) active_settings.shadows = (val_i != 0);
                    else if (strcmp(key, "theme_mode") == 0) active_settings.theme_mode = (theme_mode_t)val_i;
                    else if (strcmp(key, "window_style") == 0) active_settings.window_style = (window_style_t)val_i;
                    else if (strcmp(key, "dock_position") == 0) active_settings.dock_position = (dock_position_t)val_i;
                    else if (strcmp(key, "dock_style") == 0) active_settings.dock_style = (dock_style_t)val_i;
                    else if (strcmp(key, "dock_autohide") == 0) active_settings.dock_autohide = (val_i != 0);
                    else if (strcmp(key, "menubar_style") == 0) active_settings.menubar_style = (menubar_style_t)val_i;
                    else if (strcmp(key, "font_scale") == 0) active_settings.font_scale = val_i;
                    else if (strcmp(key, "window_shadow_size") == 0) active_settings.window_shadow_size = val_i;
                }

                if (!next_line) break;
                line = next_line + 1;
            }
            active_settings.checksum = calculate_checksum(&active_settings);
            saved_settings = active_settings;
            settings_apply();
            kprintf("Settings successfully restored from %s\n", SETTINGS_CONF_PATH);
            return;
        }
    }

    if (saved_settings.magic == SETTINGS_MAGIC &&
        saved_settings.checksum == calculate_checksum(&saved_settings)) {
        active_settings = saved_settings;
        settings_apply();
        kprintf("Settings loaded from volatile backup.\n");
    } else {
        settings_reset_defaults();
        settings_apply();
    }
}

void settings_print(void) {
    kprintf("\n--- RatanaOS System Configuration ---\n");
    kprintf("UI Theme:           %s\n", theme_get_preset_name(active_settings.ui_theme));
    kprintf("Theme Mode:         %s\n", theme_get_mode_name(active_settings.theme_mode));
    kprintf("Icon Theme:         %s\n", icon_get_theme_name());
    kprintf("Accent Color:       %s\n", theme_get_accent_name(active_settings.accent));
    kprintf("UI Scaling:         %d%%\n", active_settings.ui_scale);
    kprintf("Font Size:          %s\n", font_get_size_name(active_settings.font_size));
    kprintf("Font Scaling:       %d%%\n", active_settings.font_scale);
    kprintf("Window Style:       %s\n", theme_get_window_style_name(active_settings.window_style));
    kprintf("Window Radius:      %d px\n", active_settings.window_radius);
    kprintf("Window Shadows:     %s (%d px)\n", active_settings.shadows ? "Enabled" : "Disabled", active_settings.window_shadow_size);
    kprintf("Transparency:       %s\n", active_settings.transparency ? "Enabled" : "Disabled");
    kprintf("Dock Position:      %s\n", theme_get_dock_position_name(active_settings.dock_position));
    kprintf("Dock Style:         %s\n", theme_get_dock_style_name(active_settings.dock_style));
    kprintf("Dock Auto-Hide:     %s\n", active_settings.dock_autohide ? "Enabled" : "Disabled");
    kprintf("Dock Icon Size:     %d px\n", active_settings.dock_icon_size);
    kprintf("Dock Spacing:       %d px\n", active_settings.dock_spacing);
    kprintf("Dock Magnification: %s\n", active_settings.dock_magnification ? "Enabled" : "Disabled");
    kprintf("Desktop Icons:      %s\n", active_settings.show_desktop_icons ? "Enabled" : "Disabled");
    kprintf("Desktop Icon Size:  %d px\n", active_settings.desktop_icon_size);
    kprintf("Desktop Labels:     %s\n", active_settings.show_icon_labels ? "Enabled" : "Disabled");
    kprintf("Menu Bar Style:     %s\n", theme_get_menubar_style_name(active_settings.menubar_style));
    kprintf("Config File:        %s\n", SETTINGS_CONF_PATH);
    kprintf("Storage Backend:    %s\n", settings_get_backend_name());
    kprintf("--------------------------------------\n\n");
}

settings_backend_t settings_get_backend(void) {
    return SETTINGS_BACKEND_VFS_CONF;
}

const char* settings_get_backend_name(void) {
    return "VFS Configuration Storage (/etc/ratana/settings.conf)";
}

bool settings_is_persistent(void) {
    return true;
}

