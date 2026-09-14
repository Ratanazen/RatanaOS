#!/usr/bin/env bash
# ==============================================================================
# RiOS Installer: Filesystem Formatting & Mount Module
# ==============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/partition.sh"

format_filesystems() {
    local dev="$1"
    local mode="${2:-auto}"

    if [ "$mode" = "auto" ]; then
        if [ -d "/sys/firmware/efi" ]; then
            mode="uefi"
        else
            mode="bios"
        fi
    fi

    echo "==> Formatting filesystems for $dev ($mode mode)..."

    local part_root part_esp=""
    if [ "$mode" = "uefi" ]; then
        part_esp=$(get_part_device "$dev" 1)
        part_root=$(get_part_device "$dev" 2)

        echo "Formatting EFI System Partition ($part_esp) as FAT32..."
        mkfs.vfat -F 32 -n "EFI" "$part_esp" >/dev/null
    else
        part_root=$(get_part_device "$dev" 1)
    fi

    echo "Formatting Root Partition ($part_root) as ext4..."
    mkfs.ext4 -F -L "RiOS" -q "$part_root"

    echo "Filesystem formatting completed successfully."
}

mount_target() {
    local dev="$1"
    local target_mount="${2:-/mnt/target}"
    local mode="${3:-auto}"

    if [ "$mode" = "auto" ]; then
        if [ -d "/sys/firmware/efi" ]; then
            mode="uefi"
        else
            mode="bios"
        fi
    fi

    local part_root part_esp=""
    if [ "$mode" = "uefi" ]; then
        part_esp=$(get_part_device "$dev" 1)
        part_root=$(get_part_device "$dev" 2)
    else
        part_root=$(get_part_device "$dev" 1)
    fi

    echo "==> Mounting target filesystems to $target_mount..."
    mkdir -p "$target_mount"
    mount "$part_root" "$target_mount"

    if [ "$mode" = "uefi" ] && [ -n "$part_esp" ]; then
        mkdir -p "${target_mount}/boot/efi"
        mount "$part_esp" "${target_mount}/boot/efi"
    fi

    echo "Target mounted: Root at $target_mount ${part_esp:+(EFI at $target_mount/boot/efi)}"
}

unmount_target() {
    local target_mount="${1:-/mnt/target}"
    echo "==> Unmounting all filesystems under $target_mount..."
    # Unmount binds first if any
    for bind in dev/pts dev proc sys run boot/efi; do
        if mountpoint -q "${target_mount}/${bind}" 2>/dev/null; then
            umount -l "${target_mount}/${bind}" 2>/dev/null || true
        fi
    done
    if mountpoint -q "$target_mount" 2>/dev/null; then
        umount -R "$target_mount" 2>/dev/null || true
    fi
    echo "Unmount completed."
}

if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    if [ $# -lt 1 ]; then
        echo "Usage: $0 [format|mount|unmount] <target-disk> [target-mount-point]"
        exit 1
    fi
    action="$1"
    shift
    case "$action" in
        format)
            format_filesystems "$@"
            ;;
        mount)
            mount_target "$@"
            ;;
        unmount)
            unmount_target "${1:-/mnt/target}"
            ;;
        *)
            echo "Unknown action: $action"
            exit 1
            ;;
    esac
fi
