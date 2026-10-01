#!/usr/bin/env bash
# ==============================================================================
# RiOS Installer: Desktop Environment & Server Profile Configuration Module
# Supports: Hyprland, Sway, GNOME, KDE Plasma, and Headless Server Mode
# ==============================================================================
set -euo pipefail

configure_desktop() {
    local target_mount="${1:-/mnt/target}"
    local default_session="${2:-rios-hyprland}" # rios-hyprland, rios-sway, gnome, plasma, server

    echo "==> Configuring session / profile target: ${default_session}..."

    # Check if Server mode is requested
    if [ "${default_session}" = "server" ] || [[ "${default_session}" =~ ^(server|headless|cli)$ ]]; then
        echo "--> Configuring headless/server mode target..."
        # Set default systemd target to multi-user.target (console only)
        mkdir -p "${target_mount}/etc/systemd/system"
        ln -sf /lib/systemd/system/multi-user.target "${target_mount}/etc/systemd/system/default.target"
        chroot "$target_mount" systemctl set-default multi-user.target 2>/dev/null || true

        # Disable graphical display managers if installed
        chroot "$target_mount" systemctl disable lightdm sddm gdm3 2>/dev/null || true

        # Enable core server daemons
        chroot "$target_mount" systemctl enable ssh ufw 2>/dev/null || true
        chroot "$target_mount" systemctl enable cockpit.socket 2>/dev/null || true

        # Set Server Welcome MOTD
        cat << 'MOTDEOF' > "${target_mount}/etc/motd"
==============================================================================
 Welcome to RiOS Server Infrastructure Edition (Debian 12 Bookworm)
 
 Managed Daemons & Port Configuration:
   - SSH:      Port 22   (Active)
   - Cockpit:  Port 9090 (Start with: sudo ri-server cockpit start)
   - Nginx:    Port 80   (Start with: sudo ri-server start nginx)
   
 Control Server Services:
   - sudo ri-server status
   - sudo ri-server info
   - sudo ri-server ports
==============================================================================
MOTDEOF
        echo "Server profile configured successfully."
        return 0
    fi

    # Graphical Desktop Mode: Hyprland, Sway, GNOME, KDE Plasma
    echo "--> Configuring graphical desktop environment..."
    mkdir -p "${target_mount}/etc/systemd/system"
    ln -sf /lib/systemd/system/graphical.target "${target_mount}/etc/systemd/system/default.target"
    chroot "$target_mount" systemctl set-default graphical.target 2>/dev/null || true

    # Register Wayland sessions
    mkdir -p "${target_mount}/usr/share/wayland-sessions"
    for s in hyprland sway; do
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

    # Enable ri-first-boot service if present
    if [ -f "${target_mount}/etc/systemd/system/ri-first-boot.service" ]; then
        echo "Enabling ri-first-boot.service on target..."
        chroot "$target_mount" systemctl enable ri-first-boot.service >/dev/null 2>&1 || true
    fi

    echo "Desktop session and display manager setup completed."
}

if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    configure_desktop "${1:-/mnt/target}" "${2:-rios-hyprland}"
fi
