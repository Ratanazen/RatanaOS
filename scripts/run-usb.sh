#!/bin/bash
# RatanaOS USB Creator & Runner Tool
# Usage:
#   ./scripts/run-usb.sh list                  - List available USB drives
#   ./scripts/run-usb.sh flash /dev/sdX [edition]- Flash ISO to USB drive
#   ./scripts/run-usb.sh boot [/dev/sdX]        - Test/Boot USB drive or ISO in VM

set -e

ACTION=${1:-help}
TARGET_DRIVE=$2
EDITION=${3:-live}

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

find_iso() {
    local ed=$1
    case "${ed}" in
        lite|ratana-lite)           ed="Lite" ;;
        standard|ratana-standard)   ed="Standard" ;;
        developer|ratana-developer) ed="Developer" ;;
        cyber|ratana-cyber)         ed="Cyber" ;;
        server|ratana-server)       ed="Server" ;;
        arm|ratana-arm)             ed="ARM64" ;;
        live|ratana-live)           ed="Live" ;;
    esac

    for loc in "${ROOT_DIR}/releases/RatanaOS-${ed}.iso" "${ROOT_DIR}/output/RatanaOS-${ed}.iso" "${ROOT_DIR}/releases/RatanaOS-Live.iso" "${ROOT_DIR}/releases/RatanaOS-Standard.iso" "${ROOT_DIR}/releases/RatanaOS-Cyber.iso"; do
        if [ -f "$loc" ]; then
            echo "$loc"
            return 0
        fi
    done

    find "${ROOT_DIR}/releases" "${ROOT_DIR}/output" "${ROOT_DIR}/.artifacts" -name "RatanaOS*.iso" 2>/dev/null | head -n 1
}

list_drives() {
    echo "=========================================="
    echo "   Available USB Storage Drives           "
    echo "=========================================="
    if command -v lsblk >/dev/null 2>&1; then
        lsblk -d -o NAME,SIZE,TYPE,MODEL,TRAN,RM | grep -E "usb|TRAN|1$" || lsblk -d -o NAME,SIZE,TYPE,MODEL
    else
        fdisk -l 2>/dev/null | grep "Disk /dev/" || true
    fi
    echo ""
}

flash_usb() {
    local drive=$1
    local edition=$2

    if [ -z "$drive" ]; then
        echo "❌ Error: Please specify target USB drive (e.g. /dev/sdb)."
        echo "💡 Tip: Flash to the ENTIRE drive (/dev/sdb), not a partition (/dev/sdb1) to ensure UEFI/BIOS bootability."
        echo ""
        list_drives
        exit 1
    fi

    # Check if user accidentally passed a partition (e.g. /dev/sdb1 instead of /dev/sdb)
    if echo "$drive" | grep -qE "[0-9]$"; then
        echo "⚠️  WARNING: You specified partition '$drive'. For a bootable USB, you should write to the whole drive (e.g. '${drive%%[0-9]*}')."
    fi

    # Safety check: ensure target is not main root partition
    if [[ "$drive" == "/dev/sda" ]] || [[ "$drive" == "/" ]] || [[ "$drive" == "/dev/nvme0n1" ]]; then
        echo "⛔ CAUTION: Drive $drive might be your main system disk! Aborting for safety."
        exit 1
    fi

    ISO_FILE=$(find_iso "$edition")
    if [ -z "$ISO_FILE" ] || [ ! -f "$ISO_FILE" ]; then
        echo "❌ Error: ISO file for edition '$edition' not found. Run 'make build' or 'make usb-build' first."
        exit 1
    fi

    echo "=========================================="
    echo "   Flashing RatanaOS to USB Drive        "
    echo "=========================================="
    echo "Source ISO   : ${ISO_FILE}"
    echo "Target Drive : ${drive}"
    echo ""

    if [ "$EUID" -ne 0 ]; then
        echo "🔑 Escalating to root to write to USB drive..."
        sudo dd if="${ISO_FILE}" of="${drive}" bs=4M status=progress conv=fdatasync
        sudo sync
    else
        dd if="${ISO_FILE}" of="${drive}" bs=4M status=progress conv=fdatasync
        sync
    fi

    echo ""
    echo "🎉 Flashing complete! USB drive ${drive} is ready to boot."
}

boot_usb() {
    local drive=$1
    if [ -z "$drive" ]; then
        ISO_FILE=$(find_iso "$EDITION")
        if [ -z "$ISO_FILE" ] || [ ! -f "$ISO_FILE" ]; then
            echo "❌ Error: No ISO file found to boot."
            exit 1
        fi
        echo "🚀 Booting ISO directly in QEMU VM: ${ISO_FILE}..."
        qemu-system-x86_64 -m 4096 -smp 4 -cdrom "${ISO_FILE}" -vga virtio -machine accel=kvm:tcg || \
        qemu-system-x86_64 -m 2048 -cdrom "${ISO_FILE}" -nographic
        exit 0
    fi

    echo "🚀 Booting physical USB Drive (${drive}) in QEMU VM..."
    if [ -r "${drive}" ]; then
        qemu-system-x86_64 -m 4096 -smp 4 -drive "file=${drive},format=raw,readonly=on" -vga virtio -machine accel=kvm:tcg || \
        qemu-system-x86_64 -m 2048 -drive "file=${drive},format=raw,readonly=on" -nographic || \
        sudo qemu-system-x86_64 -m 4096 -smp 4 -hda "${drive}" -vga virtio -machine accel=kvm:tcg
    elif [ "$EUID" -ne 0 ]; then
        sudo qemu-system-x86_64 -m 4096 -smp 4 -hda "${drive}" -vga virtio -machine accel=kvm:tcg || \
        sudo qemu-system-x86_64 -m 2048 -hda "${drive}" -nographic
    else
        qemu-system-x86_64 -m 4096 -smp 4 -hda "${drive}" -vga virtio -machine accel=kvm:tcg || \
        qemu-system-x86_64 -m 2048 -hda "${drive}" -nographic
    fi
}

case "$ACTION" in
    list)
        list_drives
        ;;
    flash)
        flash_usb "$TARGET_DRIVE" "$EDITION"
        ;;
    boot|run)
        boot_usb "$TARGET_DRIVE"
        ;;
    *)
        echo "RatanaOS USB Management Tool"
        echo "Usage:"
        echo "  $0 list                     List connected USB drives"
        echo "  $0 flash /dev/sdX [edition] Flash ISO to USB drive (e.g. /dev/sdb)"
        echo "  $0 boot [/dev/sdX]          Test run USB drive or ISO in QEMU"
        ;;
esac
