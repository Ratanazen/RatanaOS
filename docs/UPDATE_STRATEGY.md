# 🔄 RatanaOS Update & Upgrade Strategy (Phase 13)

## APT Repository Architecture

RatanaOS uses **standard Debian Bookworm mirrors exclusively** for all
system packages. No custom RatanaOS APT repository is maintained.

### /etc/apt/sources.list (Installed System)
\`\`\`
deb http://deb.debian.org/debian bookworm main contrib non-free non-free-firmware
deb http://deb.debian.org/debian bookworm-updates main contrib non-free non-free-firmware
deb http://security.debian.org/debian-security bookworm-security main contrib non-free non-free-firmware
\`\`\`

### Why No Custom RatanaOS APT Repository?
RatanaOS's custom layer consists of:
- GTK/XFWM themes (static files in \`/usr/share/themes/\`)
- FreeDesktop icon suites (static PNGs in \`/usr/share/icons/\`)
- Configuration files (\`/etc/os-release\`, \`/etc/skel/.config/\`, etc.)
- Plymouth/GRUB theme assets (non-executable data)
- Calamares branding (static config)

These are **overlay files baked into the ISO at build time** and do not
change between Debian point releases. A dedicated APT repository for
these files would add infrastructure maintenance burden with minimal
benefit for the current project scale.

**Decision**: No custom RatanaOS APT repository is maintained at this time.
Theme/branding updates are delivered via new ISO releases. This decision
will be revisited if RatanaOS-specific packages require post-install
updates (e.g., a RatanaOS settings daemon or theme auto-updater).

---

## Keeping the System Current

### Routine Security & Package Updates
\`\`\`bash
sudo apt update && sudo apt upgrade
\`\`\`
This pulls updates directly from Debian's mirrors. RatanaOS's theme
layer is unaffected — custom files in \`/usr/share/themes/\`,
\`/usr/share/icons/\`, and \`/etc/os-release\` are not managed by any
Debian package and will not be overwritten by \`apt upgrade\`.

### Full Distribution Upgrade
\`\`\`bash
sudo apt update && sudo apt full-upgrade
\`\`\`
Tested and verified in Phase 6 (QA Parity Checklist item #2):
the system remains bootable and correctly themed after a full upgrade.

### Major Debian Release Upgrade (e.g., Bookworm → Trixie)
When Debian releases a new stable version:
1. Update \`/etc/apt/sources.list\` to point to the new release codename.
2. Run \`sudo apt update && sudo apt full-upgrade\`.
3. RatanaOS themes and branding survive — they are independent of
   Debian's release-specific packages.
4. Re-verify against \`docs/PARITY_CHECKLIST.md\`.

---

## Theme Preservation Across Updates

| RatanaOS Component | Location | Managed by APT? | Survives \`apt upgrade\`? |
| :--- | :--- | :---: | :---: |
| GTK Theme | \`/usr/share/themes/RatanaOS*/\` | No | **Yes** |
| XFWM Theme | \`/usr/share/themes/RatanaOS*/xfwm4/\` | No | **Yes** |
| Icon Themes | \`/usr/share/icons/RatanaOS-*/\` | No | **Yes** |
| Wallpaper | \`/usr/share/backgrounds/ratanaos/\` | No | **Yes** |
| Plymouth Theme | \`/usr/share/plymouth/themes/ratanaos/\` | No | **Yes** |
| GRUB Theme | \`/boot/grub/themes/ratanaos/\` | No | **Yes** |
| \`/etc/os-release\` | \`/etc/os-release\` | No (dpkg-conffile) | **Yes** (kept as user modification) |
| Desktop Config | \`/etc/skel/.config/\` | No | **Yes** (only affects new users) |
