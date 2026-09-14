#!/usr/bin/env bash
# ==============================================================================
# RiOS Installer: Hostname, Networking & Localization Module
# ==============================================================================
set -euo pipefail

configure_network() {
    local target_mount="${1:-/mnt/target}"
    local hostname="${2:-rios}"
    local timezone="${3:-Asia/Phnom_Penh}"

    echo "==> Configuring networking and host identity for '$hostname'..."

    # Set hostname
    echo "$hostname" > "${target_mount}/etc/hostname"

    # Configure /etc/hosts
    cat << HEOF > "${target_mount}/etc/hosts"
127.0.0.1   localhost
127.0.1.1   $hostname

# The following lines are desirable for IPv6 capable hosts
::1         localhost ip6-localhost ip6-loopback
ff02::1     ip6-allnodes
ff02::2     ip6-allrouters
HEOF

    # Configure timezone
    if [ -f "${target_mount}/usr/share/zoneinfo/${timezone}" ]; then
        echo "$timezone" > "${target_mount}/etc/timezone"
        ln -sf "/usr/share/zoneinfo/${timezone}" "${target_mount}/etc/localtime"
    fi

    # Enable NetworkManager in target chroot
    if [ -x "${target_mount}/usr/sbin/NetworkManager" ]; then
        echo "Enabling NetworkManager service..."
        chroot "$target_mount" systemctl enable NetworkManager >/dev/null 2>&1 || true
    fi

    echo "Network and host identity configuration completed."
}

if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    configure_network "${1:-/mnt/target}" "${2:-rios}" "${3:-Asia/Phnom_Penh}"
fi
