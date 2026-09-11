#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Installer: Dynamic & Safe Disk Enumeration Module
# ==============================================================================
set -euo pipefail

# Get booted live medium device to strictly exclude it
get_live_device() {
    local live_dev=""
    live_dev=$(findmnt -n -o SOURCE /run/live/medium 2>/dev/null || true)
    if [ -z "$live_dev" ]; then
        live_dev=$(lsblk -o NAME,MOUNTPOINT -rn | awk '$2 ~ /^\/run\/live/ {print "/dev/" $1}' | head -n 1)
    fi
    # Strip partition number to get parent disk
    if [ -n "$live_dev" ]; then
        echo "$live_dev" | sed -E 's/p?[0-9]+$//'
    fi
}

# Enumerate installable block devices (excluding loop, ram, zram, and live medium)
list_disks() {
    local live_parent
    live_parent=$(get_live_device)

    lsblk -d -n -o PATH,TYPE,SIZE,MODEL,TRAN 2>/dev/null | while read -r path type size model tran; do
        # Only accept disks
        [ "$type" != "disk" ] && continue
        # Ignore loop, ram, zram
        [[ "$path" =~ ^/dev/(loop|ram|zram) ]] && continue
        # Ignore live installation medium
        [ -n "$live_parent" ] && [ "$path" = "$live_parent" ] && continue

        echo "$path"
    done
}

# Display rich inspection details for a selected target disk
show_disk_details() {
    local dev="$1"
    if [ ! -b "$dev" ]; then
        echo "Error: Device '$dev' is not a valid block device." >&2
        return 1
    fi

    local size model tran vendor
    size=$(lsblk -d -n -o SIZE "$dev" 2>/dev/null || echo "Unknown")
    model=$(lsblk -d -n -o MODEL "$dev" 2>/dev/null || echo "Generic Disk")
    tran=$(lsblk -d -n -o TRAN "$dev" 2>/dev/null || echo "internal")
    vendor=$(lsblk -d -n -o VENDOR "$dev" 2>/dev/null || echo "")

    echo "============================================================"
    echo "  TARGET DISK SPECIFICATIONS"
    echo "============================================================"
    echo "  Device:        $dev"
    echo "  Model/Vendor:  $vendor $model"
    echo "  Capacity:      $size"
    echo "  Bus/Transport: $tran"
    echo "------------------------------------------------------------"
    echo "  Existing Partitions:"
    lsblk -n -o NAME,SIZE,FSTYPE,LABEL,MOUNTPOINT "$dev" | sed 's/^/    /' || echo "    (No existing partition table)"
    echo "============================================================"
}

# Safety verification check
is_safe_disk() {
    local dev="$1"
    local live_parent
    live_parent=$(get_live_device)

    if [ ! -b "$dev" ]; then
        return 1
    fi
    if [ -n "$live_parent" ] && [ "$dev" = "$live_parent" ]; then
        echo "SAFETY ALERT: Device '$dev' is the active Live installer medium!" >&2
        return 1
    fi
    # Check if any partition on this disk is mounted as root /
    if lsblk -n -o MOUNTPOINT "$dev" 2>/dev/null | grep -E '^/$' >/dev/null; then
        echo "SAFETY ALERT: Device '$dev' is mounted as host root /!" >&2
        return 1
    fi
    return 0
}

# If executed directly, list available disks
if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    echo "Available installation disks:"
    for d in $(list_disks); do
        echo " - $d ($(lsblk -d -n -o SIZE,MODEL "$d" 2>/dev/null))"
    done
fi
