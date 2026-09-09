# RatanaOS — System Settings Specification & User Manual

## 1. Overview

System Settings (`src/kernel/settings.c`, `src/include/settings.h`, `src/kernel/gui.c`) provides a unified, macOS Sequoia-style control center for desktop appearance, hardware display scaling, typography, window frames, dock layout, and system preferences.

```text
┌─────────────────────────────────────────────────────────────┐
│ ● ● ●  System Settings                                      │
├─────────────────┬───────────────────────────────────────────┤
│ [ Appearance  ] │ Theme Mode: ( ) Light  (*) Dark  ( ) Auto │
│ [ Themes      ] │ Accent: [Blue] [Purple] [Pink] [Red] ...  │
│ [ Displays    ] │                                           │
│ [ Typography  ] │ Preview:                                  │
│ [ Windows     ] │ ┌───────────────────────────────────────┐ │
│ [ Dock        ] │ │ Preview Sample Card                   │ │
│ [ Menu Bar    ] │ │ Dynamic theme token preview           │ │
│ [ Icons       ] │ └───────────────────────────────────────┘ │
│ [ About       ] │ [ Apply Changes ]        [ Save & Exit ]  │
└─────────────────┴───────────────────────────────────────────┘
```

---

## 2. Preference Categories

| Category | Description & Interactive Controls |
| :--- | :--- |
| **1. Appearance** | Light / Dark / Auto (RTC) mode selector; 8 accent color swatches (Blue, Purple, Pink, Red, Orange, Yellow, Green, Graphite). |
| **2. Themes** | Preset gallery: RatanaOS Dark, RatanaOS Light, macOS Dark, macOS Light. |
| **3. Displays** | Global display scale selector: 100%, 125%, 150%, 200%. Live resolution info. |
| **4. Typography** | Font scale slider (80%, 100%, 125%, 150%); semantic font role size preview card. |
| **5. Windows** | Window style picker: macOS Sequoia, Classic 2D, Minimalist, Acrylic Glass; shadow toggles. |
| **6. Dock** | Position (Bottom, Left, Right); Dock style (Glass, Classic, Transparent, Compact); Magnification toggle; Auto-hide toggle. |
| **7. Menu Bar** | Menu bar style picker (Glass Translucent, Solid Opaque, Fully Transparent). |
| **8. Icons** | Icon suite switcher: WhiteSur (macOS Big Sur style) vs MacTahoe (Modern Sequoia style). |
| **9. About** | System information summary: RatanaOS 64-bit kernel version, CPU, RAM usage, storage mounts. |

---

## 3. Configuration File Format (`/etc/ratana/settings.conf`)

Settings are stored in key-value format on the VFS at `/etc/ratana/settings.conf` with a verification checksum.

### Sample Configuration

```ini
theme_type=0
theme_mode=1
accent_color=0x0a84ff
dock_style=0
dock_position=0
dock_magnify=1
dock_autohide=0
dock_size=48
font_scale=100
ui_scale=100
icon_theme=0
window_style=0
menubar_style=0
checksum=0x1a4f
```

### Configuration Keys Reference

| Key | Type | Valid Range / Values | Default |
| :--- | :---: | :--- | :---: |
| `theme_type` | Integer | `0`=Ratana Dark, `1`=Ratana Light, `2`=macOS Dark, `3`=macOS Light | `0` |
| `theme_mode` | Integer | `0`=Light, `1`=Dark, `2`=Auto (RTC) | `1` |
| `accent_color`| Hex 32 | `0x00000000` - `0x00FFFFFF` | `0x0A84FF` |
| `dock_style` | Integer | `0`=Glass, `1`=Classic, `2`=Transparent, `3`=Compact | `0` |
| `dock_position`| Integer| `0`=Bottom, `1`=Left, `2`=Right | `0` |
| `dock_magnify`| Boolean| `0` (Disabled), `1` (Enabled) | `1` |
| `dock_autohide`| Boolean| `0` (Disabled), `1` (Enabled) | `0` |
| `dock_size` | Integer | `32` to `96` pixels | `48` |
| `font_scale` | Integer | `80`, `100`, `125`, `150` percent | `100` |
| `ui_scale` | Integer | `100`, `125`, `150`, `175`, `200` percent | `100` |
| `icon_theme` | Integer | `0`=WhiteSur, `1`=MacTahoe | `0` |
| `window_style`| Integer| `0`=macOS, `1`=Classic, `2`=Minimal, `3`=Transparent | `0` |
| `menubar_style`| Integer| `0`=Glass, `1`=Solid, `2`=Transparent | `0` |
| `checksum` | Hex 16 | 16-bit additive checksum of all configuration keys | Computed |

---

## 4. Integrity and Defaults Recovery

1. **Checksum Protection**: When `/etc/ratana/settings.conf` is loaded during `settings_init()`, the parser validates the 16-bit checksum.
2. **Safe Fallback**: If the file is missing, corrupt, or has an invalid checksum, `settings_init()` silently applies safe defaults without failing boot.
3. **Reset to Defaults**: System Settings contains a "Reset Defaults" action that deletes or overwrites `/etc/ratana/settings.conf` with default values.
