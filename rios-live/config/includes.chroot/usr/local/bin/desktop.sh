#!/usr/bin/env bash
# ==============================================================================
# RiOS Installer: Desktop Environment & First-Boot Service Module
# ==============================================================================
set -euo pipefail

configure_desktop() {
    local target_mount="${1:-/mnt/target}"
    local default_session="${2:-rios-hyprland}" # rios-hyprland, rios-niri, rios-sway

    echo "==> Configuring default desktop environment ($default_session)..."

    # Register Wayland sessions
    mkdir -p "${target_mount}/usr/share/wayland-sessions"
    for s in hyprland niri sway; do
        cat << SEOF > "${target_mount}/usr/share/wayland-sessions/rios-${s}.desktop"
[Desktop Entry]
Name=RiOS ${s^}
Comment=RiOS ${s^} Wayland Session
Exec=${s}
Type=Application
DesktopNames=RiOS;${s^};Wayland
Keywords=tiling;wayland;compositor;
SEOF
    done

    # Configure LightDM default session if LightDM is present
    if [ -d "${target_mount}/etc/lightdm" ]; then
        mkdir -p "${target_mount}/etc/lightdm/lightdm.conf.d"
        cat << LEOF > "${target_mount}/etc/lightdm/lightdm.conf.d/50-rios-session.conf"
[Seat:*]
user-session=${default_session}
autologin-session=${default_session}
LEOF
        chroot "$target_mount" systemctl enable lightdm >/dev/null 2>&1 || true
    fi

    # Enable ri-first-boot service
    if [ -f "${target_mount}/etc/systemd/system/ri-first-boot.service" ]; then
        echo "Enabling ri-first-boot.service on target..."
        chroot "$target_mount" systemctl enable ri-first-boot.service >/dev/null 2>&1 || true
    fi

    echo "Desktop session and display manager setup completed."
}

if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    configure_desktop "${1:-/mnt/target}" "${2:-rios-hyprland}"
fi
