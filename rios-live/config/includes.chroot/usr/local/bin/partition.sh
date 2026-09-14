#!/usr/bin/env bash
# ==============================================================================
# RiOS Installer: Partitioning Engine (UEFI GPT / BIOS MBR)
# ==============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/disk.sh"

get_part_device() {
    local disk="$1"
    local part_num="$2"
    if [[ "$disk" =~ [0-9]$ ]]; then
        echo "${disk}p${part_num}"
    else
        echo "${disk}${part_num}"
    fi
}

partition_disk() {
    local dev="$1"
    local mode="${2:-auto}" # auto, uefi, bios

    if ! is_safe_disk "$dev"; then
        echo "Error: Partitioning aborted. Device '$dev' failed safety verification." >&2
        return 1
    fi

    # Determine firmware mode if set to auto
    if [ "$mode" = "auto" ]; then
        if [ -d "/sys/firmware/efi" ]; then
            mode="uefi"
        else
            mode="bios"
        fi
    fi

    echo "==> Preparing to partition $dev in $mode mode..."

    # Unmount any existing partitions on this disk
    local mounted_parts
    mounted_parts=$(lsblk -n -o MOUNTPOINT "$dev" 2>/dev/null | grep -v '^$' || true)
    if [ -n "$mounted_parts" ]; then
        echo "Unmounting active partitions on $dev..."
        for p in $(lsblk -n -o PATH "$dev" 2>/dev/null); do
            umount -f "$p" 2>/dev/null || true
        done
    fi

    # Wipe existing partition table signatures
    wipefs -a "$dev" >/dev/null 2>&1 || true

    if [ "$mode" = "uefi" ]; then
        echo "Creating GPT partition table on $dev (UEFI)..."
        parted -s "$dev" mklabel gpt
        echo "Creating 512MB EFI System Partition..."
        parted -s "$dev" mkpart "EFI" fat32 1MiB 513MiB
        parted -s "$dev" set 1 esp on
        echo "Creating Root ext4 Partition..."
        parted -s "$dev" mkpart "RiOS" ext4 513MiB 100%
    else
        echo "Creating MBR partition table on $dev (BIOS/Legacy)..."
        parted -s "$dev" mklabel msdos
        echo "Creating Bootable Root Partition..."
        parted -s "$dev" mkpart primary ext4 1MiB 100%
        parted -s "$dev" set 1 boot on
    fi

    # Allow kernel partition table re-read
    partprobe "$dev" >/dev/null 2>&1 || true
    sleep 1

    local part_esp part_root
    if [ "$mode" = "uefi" ]; then
        part_esp=$(get_part_device "$dev" 1)
        part_root=$(get_part_device "$dev" 2)
        echo "Partitioning complete: ESP=$part_esp, ROOT=$part_root"
    else
        part_root=$(get_part_device "$dev" 1)
        echo "Partitioning complete: ROOT=$part_root"
    fi
}

if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    if [ $# -lt 1 ]; then
        echo "Usage: $0 <target-disk> [auto|uefi|bios]"
        exit 1
    fi
    partition_disk "$1" "${2:-auto}"
fi
