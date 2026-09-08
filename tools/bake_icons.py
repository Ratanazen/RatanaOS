#!/usr/bin/env python3
import os
import subprocess
from PIL import Image

ICONS_MAP = {
    'finder': 'src/apps/scalable/file-manager.svg',
    'launchpad': 'src/apps/scalable/AppImageLauncher.svg',
    'safari': 'src/apps/scalable/safari.svg',
    'terminal': 'src/apps/scalable/terminal.svg',
    'sysmon': 'src/apps/scalable/utilities-system-monitor.svg',
    'calculator': 'src/apps/scalable/calc.svg',
    'paint': 'src/apps/scalable/gimp.svg',
    'notes': 'src/apps/scalable/accessories-text-editor.svg',
    'music': 'src/apps/scalable/gnome-music.svg',
    'settings': 'src/apps/scalable/preferences-system.svg',
    'appstore': 'src/apps/scalable/softwarecenter.svg',
    'about': 'src/apps/scalable/tapple.svg',
    'trash': 'src/places/scalable/user-trash.svg',
    'drive': 'src/devices/scalable/drive-harddisk.svg',
    'folder': 'src/places/scalable/folder.svg'
}

THEMES = [
    ('whitesur', 'WhiteSur', '/tmp/WhiteSur-icon-theme'),
    ('mactahoe', 'MacTahoe', '/tmp/MacTahoe-icon-theme')
]

def rasterize_svg(svg_path, size):
    tmp_png = f'/tmp/raster_{size}_{os.path.basename(svg_path)}.png'
    cmd = ['rsvg-convert', '-w', str(size), '-h', str(size), '-f', 'png', svg_path, '-o', tmp_png]
    subprocess.run(cmd, check=True)
    img = Image.open(tmp_png).convert('RGBA')
    raw_pixels = list(img.getdata())
    # Format: 0xAARRGGBB
    pixels = [((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF) for r, g, b, a in raw_pixels]
    if os.path.exists(tmp_png):
        os.remove(tmp_png)
    return pixels

def main():
    os.makedirs('src/include', exist_ok=True)
    os.makedirs('src/drivers', exist_ok=True)

    header_content = """#ifndef ICONS_ASSETS_H
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
"""

    with open('src/include/icons_assets.h', 'w') as f:
        f.write(header_content)
    print("Generated src/include/icons_assets.h")

    c_content = []
    c_content.append('#include "../include/icons_assets.h"\n')
    c_content.append('// Auto-generated 32-bit ARGB Icon Asset Tables for WhiteSur and MacTahoe\n\n')

    theme_tables = {}

    for t_id, t_name, t_path in THEMES:
        print(f"Baking {t_name} icons...")
        theme_tables[t_id] = []
        for icon_key, rel_svg in ICONS_MAP.items():
            svg_full = os.path.join(t_path, rel_svg)
            if not os.path.exists(svg_full):
                print(f"Warning: {svg_full} not found, checking fallback...")
                continue
            
            # 48x48 icon
            pixels_48 = rasterize_svg(svg_full, 48)
            var_name = f"icon_{t_id}_{icon_key}_48"
            theme_tables[t_id].append((icon_key, var_name, 48))

            c_content.append(f"static const uint32_t {var_name}[48 * 48] = {{")
            # Write 8 pixels per line
            for i in range(0, len(pixels_48), 8):
                chunk = pixels_48[i:i+8]
                c_content.append("    " + ", ".join(f"0x{p:08X}" for p in chunk) + ",")
            c_content.append("};\n")

            # If About, also bake 64x64
            if icon_key == 'about':
                pixels_64 = rasterize_svg(svg_full, 64)
                var_64 = f"icon_{t_id}_about_64"
                c_content.append(f"static const uint32_t {var_64}[64 * 64] = {{")
                for i in range(0, len(pixels_64), 8):
                    chunk = pixels_64[i:i+8]
                    c_content.append("    " + ", ".join(f"0x{p:08X}" for p in chunk) + ",")
                c_content.append("};\n")

    # Accessor arrays
    for t_id, t_name, _ in THEMES:
        c_content.append(f"static const uint32_t* const {t_id}_assets[ICON_ID_COUNT] = {{")
        c_content.append(f"    [ICON_ID_FINDER]     = icon_{t_id}_finder_48,")
        c_content.append(f"    [ICON_ID_LAUNCHPAD]  = icon_{t_id}_launchpad_48,")
        c_content.append(f"    [ICON_ID_SAFARI]     = icon_{t_id}_safari_48,")
        c_content.append(f"    [ICON_ID_TERMINAL]   = icon_{t_id}_terminal_48,")
        c_content.append(f"    [ICON_ID_SYSMON]     = icon_{t_id}_sysmon_48,")
        c_content.append(f"    [ICON_ID_CALCULATOR] = icon_{t_id}_calculator_48,")
        c_content.append(f"    [ICON_ID_PAINT]      = icon_{t_id}_paint_48,")
        c_content.append(f"    [ICON_ID_NOTES]      = icon_{t_id}_notes_48,")
        c_content.append(f"    [ICON_ID_MUSIC]      = icon_{t_id}_music_48,")
        c_content.append(f"    [ICON_ID_SETTINGS]   = icon_{t_id}_settings_48,")
        c_content.append(f"    [ICON_ID_APPSTORE]   = icon_{t_id}_appstore_48,")
        c_content.append(f"    [ICON_ID_ABOUT]      = icon_{t_id}_about_48,")
        c_content.append(f"    [ICON_ID_TRASH]      = icon_{t_id}_trash_48,")
        c_content.append(f"    [ICON_ID_DRIVE]      = icon_{t_id}_drive_48,")
        c_content.append(f"    [ICON_ID_FOLDER]     = icon_{t_id}_folder_48,")
        c_content.append(f"    [ICON_ID_ABOUT_64]   = icon_{t_id}_about_64,")
        c_content.append("};\n")

    c_content.append("""const uint32_t* icons_asset_get(icon_theme_id_t theme, icon_id_t icon) {
    if (icon < 0 || icon >= ICON_ID_COUNT) return NULL;
    if (theme == ICON_THEME_WHITESUR) {
        return whitesur_assets[icon];
    } else if (theme == ICON_THEME_MACTAHOE) {
        return mactahoe_assets[icon];
    }
    return NULL;
}

const char* icons_theme_get_name(icon_theme_id_t theme) {
    switch (theme) {
        case ICON_THEME_WHITESUR: return "WhiteSur (Sequoia Classic)";
        case ICON_THEME_MACTAHOE: return "MacTahoe (Next-Gen Dark)";
        case ICON_THEME_VECTOR:   return "Vector Procedural";
        default: return "Default";
    }
}
""")

    with open('src/drivers/icons_assets.c', 'w') as f:
        f.write("\n".join(c_content))
    print("Generated src/drivers/icons_assets.c successfully!")

if __name__ == '__main__':
    main()
