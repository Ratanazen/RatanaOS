#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

echo "=== Building RatanaOS 64-bit Bootable ISO ==="
cd "$ROOT_DIR"

make

mkdir -p iso/boot/grub
cp build/ratanaos.bin iso/boot/ratanaos.bin

if which grub-mkrescue >/dev/null 2>&1; then
    grub-mkrescue -o build/ratanaos.iso iso
    echo ">>> Successfully built ISO: build/ratanaos.iso <<<"
else
    echo "Error: grub-mkrescue is not installed. Run 'sudo pacman -S grub xorriso' or 'sudo apt install grub-common xorriso'."
    exit 1
fi
