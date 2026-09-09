#!/usr/bin/env python3
import os
import re
from PIL import Image

SRC_FILE = "src/drivers/icons_assets.c"
TARGET_BASE = "ratanaos-live/config/includes.chroot/usr/share/icons"

THEMES = {
    'whitesur': 'RatanaOS-WhiteSur',
    'mactahoe': 'RatanaOS-MacTahoe'
}

SIZES = [16, 22, 24, 32, 48, 64, 128, 256]

ICON_ALIASES = {
    'finder': ['system-file-manager', 'file-manager', 'org.xfce.thunar', 'thunar', 'desktop'],
    'launchpad': ['applications-other', 'xfce4-appfinder', 'xfce-system-menu', 'start-here', 'apple-logo'],
    'safari': ['web-browser', 'firefox-esr', 'firefox', 'browser', 'internet-web-browser'],
    'terminal': ['utilities-terminal', 'terminal', 'xfce4-terminal', 'console'],
    'sysmon': ['utilities-system-monitor', 'taskmanager', 'xfce4-taskmanager', 'htop', 'gnome-system-monitor'],
    'calculator': ['accessories-calculator', 'calc', 'gnome-calculator'],
    'paint': ['gimp', 'accessories-image-viewer', 'drawing'],
    'notes': ['accessories-text-editor', 'mousepad', 'gedit', 'text-editor'],
    'music': ['multimedia-player', 'gnome-music', 'audio-player'],
    'settings': ['preferences-system', 'xfce4-settings-manager', 'preferences-desktop', 'preferences-other'],
    'appstore': ['softwarecenter', 'system-software-install', 'calamares', 'package-manager'],
    'about': ['help-about', 'distributor-logo', 'ratanaos', 'system-help', 'ratanaos-logo'],
    'trash': ['user-trash', 'user-trash-full', 'trash-empty'],
    'drive': ['drive-harddisk', 'system-devices', 'drive-multidisk'],
    'folder': ['folder', 'inode-directory', 'folder-open']
}

def main():
    with open(SRC_FILE, 'r') as f:
        content = f.read()

    pattern = re.compile(r'static\s+const\s+uint32_t\s+icon_([a-z0-9]+)_([a-z0-9_]+)_(\d+)\[.*?\]\s*=\s*\{(.*?)\};', re.DOTALL)
    matches = pattern.findall(content)
    print(f"[*] Found {len(matches)} icon arrays in {SRC_FILE}")

    for theme_key, icon_name, base_size_str, body in matches:
        if theme_key not in THEMES:
            continue
        theme_name = THEMES[theme_key]
        base_size = int(base_size_str)
        if base_size != 48 and base_size != 64:
            continue

        hex_tokens = re.findall(r'0x[0-9a-fA-F]+', body)
        if len(hex_tokens) != base_size * base_size:
            continue

        # Convert ARGB to RGBA
        img_base = Image.new('RGBA', (base_size, base_size))
        pixels = []
        for h in hex_tokens:
            val = int(h, 16)
            a = (val >> 24) & 0xFF
            r = (val >> 16) & 0xFF
            g = (val >> 8) & 0xFF
            b = val & 0xFF
            pixels.append((r, g, b, a))
        img_base.putdata(pixels)

        # Generate all sizes
        for size in SIZES:
            out_dir = os.path.join(TARGET_BASE, theme_name, f"{size}x{size}/apps")
            os.makedirs(out_dir, exist_ok=True)

            if size == base_size:
                scaled_img = img_base
            else:
                scaled_img = img_base.resize((size, size), Image.Resampling.LANCZOS)

            # Save primary icon
            scaled_img.save(os.path.join(out_dir, f"{icon_name}.png"))

            # Save aliases
            for alias in ICON_ALIASES.get(icon_name, []):
                scaled_img.save(os.path.join(out_dir, f"{alias}.png"))

    # Generate index.theme
    for theme_key, theme_name in THEMES.items():
        theme_dir = os.path.join(TARGET_BASE, theme_name)
        dir_entries = [f"{s}x{s}/apps" for s in SIZES]
        dirs_str = ",".join(dir_entries)

        index_content = f"""[Icon Theme]
Name={theme_name}
Comment=RatanaOS {'WhiteSur' if 'WhiteSur' in theme_name else 'MacTahoe'} macOS Icon Suite
Inherits=Adwaita,gnome,hicolor
Directories={dirs_str}

"""
        for s in SIZES:
            index_content += f"""[{s}x{s}/apps]
Size={s}
Context=Applications
Type=Fixed

"""
        with open(os.path.join(theme_dir, "index.theme"), "w") as f:
            f.write(index_content)
        print(f"[+] Successfully generated FreeDesktop icon theme: {theme_name}")

if __name__ == "__main__":
    main()
