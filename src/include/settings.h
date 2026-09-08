#ifndef SETTINGS_H
#define SETTINGS_H

#include "types.h"
#include "theme.h"
#include "icons_assets.h"
#include "font.h"

#define SETTINGS_MAGIC 0x52415441 // 'RATA'

typedef struct {
    uint32_t magic;
    uint32_t version;

    ui_theme_preset_t  ui_theme;
    icon_theme_id_t    icon_theme;
    ui_accent_color_t  accent;
    int                ui_scale;
    font_size_t        font_size;

    int                dock_icon_size;
    int                dock_spacing;
    bool               dock_magnification;

    int                window_radius;
    bool               transparency;
    bool               shadows;

    uint32_t           checksum;
} settings_t;

void settings_init(void);
settings_t* settings_get(void);
void settings_apply(void);
void settings_save(void);
void settings_load(void);
void settings_reset_defaults(void);
void settings_print(void);

#endif // SETTINGS_H
