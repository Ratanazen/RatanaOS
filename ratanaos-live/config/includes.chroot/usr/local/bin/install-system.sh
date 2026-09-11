#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Installer: Base System Deployment & Live-Boot Removal Module
# ==============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/filesystem.sh"

bind_chroot() {
    local target="$1"
    for dir in dev dev/pts proc sys run; do
        mkdir -p "${target}/${dir}"
        mount --bind "/${dir}" "${target}/${dir}"
    done
}

unbind_chroot() {
    local target="$1"
    for dir in run sys proc dev/pts dev; do
        if mountpoint -q "${target}/${dir}" 2>/dev/null; then
            umount -l "${target}/${dir}" 2>/dev/null || true
        fi
    done
}

install_system_files() {
    local target_mount="${1:-/mnt/target}"
    local squashfs_path=""

    for path in \
        /run/live/medium/live/filesystem.squashfs \
        /lib/live/mount/medium/live/filesystem.squashfs \
        /run/live/rootfs/filesystem.squashfs \
        /workspace/ratanaos-live/binary/live/filesystem.squashfs; do
        if [ -f "$path" ]; then
            squashfs_path="$path"
            break
        fi
    done

    echo "==> Deploying RatanaOS system files to $target_mount..."
    if [ -n "$squashfs_path" ] && command -v unsquashfs >/dev/null 2>&1; then
        echo "Extracting squashfs image ($squashfs_path)..."
        unsquashfs -f -d "$target_mount" "$squashfs_path"
    else
        echo "Synchronizing live system rootfs via rsync..."
        rsync -aHAX --info=progress2 \
            --exclude="/proc/*" \
            --exclude="/sys/*" \
            --exclude="/dev/*" \
            --exclude="/run/*" \
            --exclude="/tmp/*" \
            --exclude="/mnt/*" \
            --exclude="/media/*" \
            --exclude="/lost+found" \
            --exclude="/var/tmp/*" \
            --exclude="/var/cache/apt/archives/*.deb" \
            --exclude="/root/.cache/*" \
            --exclude="/run/live/*" \
            / "$target_mount/"
    fi

    # Create missing mount point folders
    mkdir -p "${target_mount}/proc" "${target_mount}/sys" "${target_mount}/dev" \
             "${target_mount}/run" "${target_mount}/tmp" "${target_mount}/mnt" \
             "${target_mount}/media" "${target_mount}/boot/efi"
    chmod 1777 "${target_mount}/tmp"

    echo "System files deployed successfully."
}

purge_live_components() {
    local target_mount="${1:-/mnt/target}"
    echo "==> Purging Live-Boot packages and regenerating installed initramfs..."

    bind_chroot "$target_mount"

    # Remove live packages in target chroot
    chroot "$target_mount" bash -c "
        export DEBIAN_FRONTEND=noninteractive
        apt-get update -qq 2>/dev/null || true
        apt-get purge -y -qq live-boot live-boot-doc live-config live-config-doc live-config-systemd live-tools calamares calamares-settings-debian 2>/dev/null || true
        
        # Clean up live-specific configs and hooks
        rm -rf /etc/live /lib/live 2>/dev/null || true
        rm -f /etc/sudoers.d/live /etc/sudoers.d/99_ratana_live 2>/dev/null || true
        
        # Ensure systemd default target is graphical
        systemctl set-default graphical.target 2>/dev/null || true
        
        # Regenerate initramfs without live-boot hooks
        echo 'Regenerating initramfs for installed disk boot...'
        update-initramfs -u -k all
    "

    unbind_chroot "$target_mount"
    echo "Live-Boot purge and initramfs regeneration complete."
}

if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    target="${1:-/mnt/target}"
    install_system_files "$target"
    purge_live_components "$target"
fi
