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

# Comprehensive FreeDesktop icon mappings across XFCE, GNOME, and KDE
ICON_MAPPINGS = {
    'finder': {
        'categories': ['apps', 'places'],
        'aliases': ['system-file-manager', 'file-manager', 'org.xfce.thunar', 'thunar', 'org.gnome.Nautilus', 'dolphin', 'nautilus', 'desktop']
    },
    'launchpad': {
        'categories': ['apps', 'actions'],
        'aliases': ['applications-other', 'xfce4-appfinder', 'xfce-system-menu', 'start-here', 'apple-logo', 'view-app-grid', 'org.kde.plasma.kickoff', 'gnome-main-menu']
    },
    'safari': {
        'categories': ['apps'],
        'aliases': ['web-browser', 'firefox-esr', 'firefox', 'browser', 'internet-web-browser', 'org.gnome.Epiphany', 'falkon', 'chromium']
    },
    'terminal': {
        'categories': ['apps'],
        'aliases': ['utilities-terminal', 'terminal', 'xfce4-terminal', 'org.gnome.Terminal', 'konsole', 'console', 'alacritty', 'kitty']
    },
    'sysmon': {
        'categories': ['apps'],
        'aliases': ['utilities-system-monitor', 'taskmanager', 'xfce4-taskmanager', 'htop', 'gnome-system-monitor', 'org.kde.ksysguard', 'ksysguard']
    },
    'calculator': {
        'categories': ['apps'],
        'aliases': ['accessories-calculator', 'calc', 'gnome-calculator', 'kcalc', 'org.gnome.Calculator']
    },
    'paint': {
        'categories': ['apps'],
        'aliases': ['gimp', 'accessories-image-viewer', 'drawing', 'org.gnome.eog', 'gwenview', 'kolourpaint']
    },
    'notes': {
        'categories': ['apps'],
        'aliases': ['accessories-text-editor', 'mousepad', 'gedit', 'text-editor', 'kate', 'kwrite', 'org.gnome.TextEditor']
    },
    'music': {
        'categories': ['apps', 'mimetypes'],
        'aliases': ['multimedia-player', 'gnome-music', 'audio-player', 'rhythmbox', 'elisa', 'audacious', 'audio-x-generic']
    },
    'settings': {
        'categories': ['apps', 'categories'],
        'aliases': ['preferences-system', 'xfce4-settings-manager', 'preferences-desktop', 'preferences-other', 'gnome-control-center', 'systemsettings']
    },
    'appstore': {
        'categories': ['apps'],
        'aliases': ['softwarecenter', 'system-software-install', 'calamares', 'package-manager', 'gnome-software', 'discover', 'org.kde.discover']
    },
    'about': {
        'categories': ['apps', 'actions'],
        'aliases': ['help-about', 'distributor-logo', 'ratanaos', 'system-help', 'ratanaos-logo', 'info', 'dialog-information']
    },
    'trash': {
        'categories': ['apps', 'places', 'status'],
        'aliases': ['user-trash', 'user-trash-full', 'trash-empty', 'user-trash-empty', 'trashcan_empty', 'trashcan_full']
    },
    'drive': {
        'categories': ['devices', 'places'],
        'aliases': ['drive-harddisk', 'system-devices', 'drive-multidisk', 'drive-removable-media', 'computer', 'harddrive']
    },
    'folder': {
        'categories': ['places', 'mimetypes'],
        'aliases': ['folder', 'inode-directory', 'folder-open', 'user-home', 'folder-home', 'folder-documents', 'folder-download', 'folder-pictures', 'folder-music', 'folder-videos']
    }
}

CATEGORIES = ['apps', 'places', 'devices', 'status', 'actions', 'mimetypes', 'categories']

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

        mapping = ICON_MAPPINGS.get(icon_name, {'categories': ['apps'], 'aliases': []})
        cats = mapping.get('categories', ['apps'])
        aliases = mapping.get('aliases', [])

        for size in SIZES:
            if size == base_size:
                scaled_img = img_base
            else:
                scaled_img = img_base.resize((size, size), Image.Resampling.LANCZOS)

            for cat in cats:
                out_dir = os.path.join(TARGET_BASE, theme_name, f"{size}x{size}/{cat}")
                os.makedirs(out_dir, exist_ok=True)
                scaled_img.save(os.path.join(out_dir, f"{icon_name}.png"))
                for alias in aliases:
                    scaled_img.save(os.path.join(out_dir, f"{alias}.png"))

    # Generate full FreeDesktop index.theme for both WhiteSur and MacTahoe
    for theme_key, theme_name in THEMES.items():
        theme_dir = os.path.join(TARGET_BASE, theme_name)
        dir_entries = []
        for s in SIZES:
            for cat in CATEGORIES:
                cat_path = os.path.join(theme_dir, f"{s}x{s}/{cat}")
                if os.path.exists(cat_path):
                    dir_entries.append(f"{s}x{s}/{cat}")

        dirs_str = ",".join(dir_entries)
        index_content = f"""[Icon Theme]
Name={theme_name}
Comment=RatanaOS {'WhiteSur' if 'WhiteSur' in theme_name else 'MacTahoe'} macOS Sequoia 2026 Icon Suite (FreeDesktop / GNOME / KDE / XFCE)
Inherits=Adwaita,breeze,gnome,hicolor
Directories={dirs_str}

"""
        for entry in dir_entries:
            size_part, cat_part = entry.split("/")
            s_val = size_part.split("x")[0]
            index_content += f"""[{entry}]
Size={s_val}
Context={cat_part.capitalize()}
Type=Fixed

"""
        with open(os.path.join(theme_dir, "index.theme"), "w") as f:
            f.write(index_content)
        print(f"[+] Successfully generated Full macOS 2026 FreeDesktop icon suite: {theme_name}")

if __name__ == "__main__":
    main()
