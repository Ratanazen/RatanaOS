#!/bin/bash
set -e

EDITION=${1:-standard}
ISO_FILE="output/RatanaOS-${EDITION}.iso"

if [ ! -f "$ISO_FILE" ]; then
    echo "Error: $ISO_FILE not found!"
    exit 1
fi

echo "Booting $ISO_FILE in QEMU..."
qemu-system-x86_64 -m 4096 -smp 4 \
    -cdrom "$ISO_FILE" \
    -boot d \
    -enable-kvm \
    -vga virtio \
    -display sdl
