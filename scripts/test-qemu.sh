#!/bin/bash
set -e

EDITION=${1:-Cyber}

ISO_FILE="releases/RatanaOS-${EDITION}.iso"
if [ ! -f "$ISO_FILE" ]; then
    ISO_FILE=$(find releases output .artifacts -name "RatanaOS*.iso" 2>/dev/null | head -n 1)
fi

if [ -z "$ISO_FILE" ] || [ ! -f "$ISO_FILE" ]; then
    echo "Error: Could not locate ISO file for edition '${EDITION}' in releases/ or output/!"
    exit 1
fi

echo "Booting $ISO_FILE in QEMU VM..."

QEMU_ARGS="-m 4096 -smp 4 -cdrom $ISO_FILE -boot d -vga virtio -machine accel=kvm:tcg"

# Check UEFI firmware
if [ -f "/usr/share/ovmf/OVMF.fd" ]; then
    QEMU_ARGS="$QEMU_ARGS -bios /usr/share/ovmf/OVMF.fd"
elif [ -f "/usr/share/OVMF/OVMF_CODE.fd" ]; then
    QEMU_ARGS="$QEMU_ARGS -bios /usr/share/OVMF/OVMF_CODE.fd"
fi

# Check display environment
if [ -z "$DISPLAY" ] && [ -z "$WAYLAND_DISPLAY" ]; then
    echo "ℹ️  No graphical display detected ($DISPLAY). Running QEMU in headless mode (-display none)."
    QEMU_ARGS="$QEMU_ARGS -display none -daemonize"
fi

qemu-system-x86_64 $QEMU_ARGS
echo "✅ QEMU Virtual Machine launched successfully!"
