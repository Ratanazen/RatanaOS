# 🌐 RatanaOS Localization & Khmer Language Support (Phase 8)

## Khmer Language Support

RatanaOS includes out-of-the-box support for Khmer (ខ្មែរ) as a first-class
language alongside English:

### Inherited from Debian (Automatic Translation Coverage)
The following components receive upstream Khmer translations via Debian's
l10n infrastructure (.mo / .po translation files):

| Component | Khmer Translation Source | Coverage |
| :--- | :--- | :--- |
| XFCE4 Desktop (menus, dialogs, settings) | xfce4-l10n / Debian transifex | Extensive |
| Calamares Installer (UI wizard) | calamares-l10n | Full |
| LightDM Greeter | lightdm-gtk-greeter l10n | Partial |
| System Settings (locale, keyboard) | Debian upstream | Full |
| Firefox ESR | Mozilla l10n | Full |
| LibreOffice (if installed) | libreoffice-l10n-km | Full |
| Plymouth Boot Text | plymouth l10n | Partial |
| GNOME/GTK system dialogs | gtk-l10n | Full |

### RatanaOS-Custom Strings (English-Only — Requires Manual Translation)
The following RatanaOS-authored strings are currently **English-only** and
do NOT receive automatic Debian translation:

| File | String / Content | Translation Status |
| :--- | :--- | :--- |
| `/etc/os-release` | `PRETTY_NAME="RatanaOS 1.0 (Sequoia)"` | English-only (standard practice) |
| `/etc/motd` | ASCII art banner and system summary | English-only |
| `/etc/issue` | Console login banner | English-only |
| Calamares `branding.desc` | `productName`, `versionedName`, slide text | English-only |
| Calamares `show.qml` | Welcome slideshow descriptions | English-only |
| `ratanaos-theme-switch` | Script output messages | English-only |
| Plank dock `settings` | Dock item labels | Uses icon names (language-neutral) |

> To add Khmer translations for RatanaOS-authored strings, create
> parallel translation files in \`config/includes.chroot/\` or use
> gettext \`.po\` files for scripts. This is tracked as a future enhancement.

### Fonts & Rendering
- **fonts-khmeros** and **fonts-khmeros-core**: Official KhmerOS Unicode
  typefaces verified to render correctly in XFCE panel, menus, Thunar,
  Firefox, and terminal emulators.
- **fonts-noto**: Google Noto fonts providing broad Unicode fallback
  coverage to prevent tofu/missing-glyph boxes.

### Input Method
- **IBus + M17N**: Pre-configured with Khmer (NIDA layout — the standard
  Cambodian keyboard layout).
- **Toggle Hotkey**: \`Super+Space\` (IBus default). Can be reconfigured
  via IBus Preferences (\`ibus-setup\`).
- IBus daemon auto-starts via \`~/.config/autostart/ibus-daemon.desktop\`.

### Test Procedure
1. Boot live ISO.
2. At LightDM greeter, select Khmer session language (or switch via
   XFCE Settings > Language).
3. Verify desktop menus, panel, and file manager render Khmer script
   correctly (no tofu boxes).
4. Open Mousepad text editor, press \`Super+Space\` to toggle IBus to
   Khmer input, and verify Khmer text entry works.
