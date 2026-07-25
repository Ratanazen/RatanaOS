#!/bin/bash
set -e

EDITION=${1:-standard}
# Map edition to Base name used for ISO
case "$EDITION" in
  lite)       BASE_NAME="RatanaOS-Lite" ;;
  standard)   BASE_NAME="RatanaOS-Standard" ;;
  developer)  BASE_NAME="RatanaOS-Developer" ;;
  cyber)      BASE_NAME="RatanaOS-Cyber" ;;
  live)       BASE_NAME="RatanaOS-Live" ;;
  server)     BASE_NAME="RatanaOS-Server" ;;
  arm)        BASE_NAME="RatanaOS-ARM64" ;;
  *)          BASE_NAME="RatanaOS-${EDITION^}" ;;
esac

ISO_FILE="releases/${BASE_NAME}.iso"
if [ ! -f "$ISO_FILE" ]; then
    # fallback search
    ISO_FILE=$(find releases output .artifacts -name "RatanaOS*.iso" 2>/dev/null | head -n 1)
fi

if [ -z "$ISO_FILE" ] || [ ! -f "$ISO_FILE" ]; then
    echo "Error: Could not locate ISO file for edition '${EDITION}'. Expected ${ISO_FILE}."
    exit 1
fi

echo "Booting $ISO_FILE in QEMU VM..."

QEMU_ARGS="-m 4096 -smp 4 -cdrom $ISO_FILE -boot d -vga virtio -machine accel=kvm:tcg"

# Enable KVM if available
if [ -c /dev/kvm ]; then
    QEMU_ARGS="$QEMU_ARGS -enable-kvm"
fi

# Check UEFI firmware
if [ -f "/usr/share/ovmf/OVMF.fd" ]; then
    QEMU_ARGS="$QEMU_ARGS -bios /usr/share/ovmf/OVMF.fd"
elif [ -f "/usr/share/OVMF/OVMF_CODE.fd" ]; then
    QEMU_ARGS="$QEMU_ARGS -bios /usr/share/OVMF/OVMF_CODE.fd"
fi

# Check display environment
if [ -z "$DISPLAY" ] && [ -z "$WAYLAND_DISPLAY" ]; then
    echo "ℹ️  No graphical display detected. Running QEMU in headless mode (-display none)."
    QEMU_ARGS="$QEMU_ARGS -display none -daemonize"
fi

qemu-system-x86_64 $QEMU_ARGS
echo "✅ QEMU Virtual Machine launched successfully!"
