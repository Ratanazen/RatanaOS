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
    -bios /usr/share/ovmf/OVMF.fd \
    -cdrom "$ISO_FILE" \
    -boot d \
    -machine accel=kvm:tcg \
    -vga virtio
