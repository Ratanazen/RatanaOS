#!/usr/bin/env python3
import re
import os
from PIL import Image

SRC_FILE = "src/drivers/icons_assets.c"
TARGET_BASE = "ratanaos-live/config/includes.chroot/usr/share/icons"

THEMES = {
    'whitesur': 'WhiteSur',
    'mactahoe': 'MacTahoe'
}

ICON_ALIASES = {
    'finder': ['system-file-manager', 'file-manager', 'org.xfce.thunar', 'thunar'],
    'launchpad': ['applications-other', 'xfce4-appfinder', 'xfce-system-menu'],
    'safari': ['web-browser', 'firefox-esr', 'firefox', 'browser'],
    'terminal': ['utilities-terminal', 'terminal', 'xfce4-terminal'],
    'sysmon': ['utilities-system-monitor', 'taskmanager', 'xfce4-taskmanager', 'htop'],
    'calculator': ['accessories-calculator', 'calc'],
    'paint': ['gimp', 'accessories-image-viewer'],
    'notes': ['accessories-text-editor', 'mousepad'],
    'music': ['multimedia-player', 'gnome-music'],
    'settings': ['preferences-system', 'xfce4-settings-manager', 'preferences-desktop'],
    'appstore': ['softwarecenter', 'system-software-install', 'calamares'],
    'about': ['help-about', 'distributor-logo', 'ratanaos', 'system-help'],
    'trash': ['user-trash', 'user-trash-full', 'trash-empty'],
    'drive': ['drive-harddisk', 'system-devices'],
    'folder': ['folder', 'inode-directory']
}

def parse_icons():
    with open(SRC_FILE, 'r') as f:
        content = f.read()

    # Match: static const uint32_t icon_{theme}_{name}_{size}[...] = { ... };
    pattern = re.compile(r'static\s+const\s+uint32_t\s+icon_([a-z0-9]+)_([a-z0-9_]+)_(\d+)\[.*?\]\s*=\s*\{(.*?)\};', re.DOTALL)
    matches = pattern.findall(content)

    print(f"Found {len(matches)} icon arrays in {SRC_FILE}")

    for theme_key, icon_name, size_str, body in matches:
        if theme_key not in THEMES:
            continue
        theme_name = THEMES[theme_key]
        size = int(size_str)

        # Parse hex numbers: 0xAARRGGBB
        hex_tokens = re.findall(r'0x[0-9a-fA-F]+', body)
        if len(hex_tokens) != size * size:
            print(f"Warning: size mismatch for {theme_key}_{icon_name}_{size}: expected {size*size}, got {len(hex_tokens)}")
            continue

        # Convert ARGB to RGBA
        img = Image.new('RGBA', (size, size))
        pixels = []
        for h in hex_tokens:
            val = int(h, 16)
            a = (val >> 24) & 0xFF
            r = (val >> 16) & 0xFF
            g = (val >> 8) & 0xFF
            b = val & 0xFF
            pixels.append((r, g, b, a))

        img.putdata(pixels)

        # Directories
        out_dir = os.path.join(TARGET_BASE, theme_name, f"{size}x{size}/apps")
        os.makedirs(out_dir, exist_ok=True)

        primary_path = os.path.join(out_dir, f"{icon_name}.png")
        img.save(primary_path)

        # Create aliases / symlinks
        aliases = ICON_ALIASES.get(icon_name, [])
        for alias in aliases:
            alias_path = os.path.join(out_dir, f"{alias}.png")
            img.save(alias_path)

        print(f"Generated {theme_name} {size}x{size} icon: {icon_name} (+ {len(aliases)} aliases)")

    # Create index.theme for both
    for theme_key, theme_name in THEMES.items():
        theme_dir = os.path.join(TARGET_BASE, theme_name)
        index_file = os.path.join(theme_dir, "index.theme")
        comment = "RatanaOS WhiteSur Icon Suite" if theme_key == "whitesur" else "RatanaOS MacTahoe Icon Suite"
        with open(index_file, "w") as f:
            f.write(f"""[Icon Theme]
Name={theme_name}
Comment={comment}
Inherits=Adwaita,gnome,hicolor
Directories=48x48/apps,64x64/apps

[48x48/apps]
Size=48
Context=Applications
Type=Fixed

[64x64/apps]
Size=64
Context=Applications
Type=Fixed
""")
        print(f"Created {index_file}")

if __name__ == "__main__":
    parse_icons()
