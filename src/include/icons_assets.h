#ifndef ICONS_ASSETS_H
#define ICONS_ASSETS_H

#include "types.h"

#define ICON_SIZE_48 48
#define ICON_SIZE_64 64

typedef enum {
    ICON_THEME_WHITESUR = 0,
    ICON_THEME_MACTAHOE,
    ICON_THEME_VECTOR,
    ICON_THEME_COUNT
} icon_theme_id_t;

typedef enum {
    ICON_ID_FINDER = 0,
    ICON_ID_LAUNCHPAD,
    ICON_ID_SAFARI,
    ICON_ID_TERMINAL,
    ICON_ID_SYSMON,
    ICON_ID_CALCULATOR,
    ICON_ID_PAINT,
    ICON_ID_NOTES,
    ICON_ID_MUSIC,
    ICON_ID_SETTINGS,
    ICON_ID_APPSTORE,
    ICON_ID_ABOUT,
    ICON_ID_TRASH,
    ICON_ID_DRIVE,
    ICON_ID_FOLDER,
    ICON_ID_ABOUT_64,
    ICON_ID_COUNT
} icon_id_t;

const uint32_t* icons_asset_get(icon_theme_id_t theme, icon_id_t icon);
const char* icons_theme_get_name(icon_theme_id_t theme);

#endif // ICONS_ASSETS_H
