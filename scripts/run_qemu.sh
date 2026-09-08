#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

cd "$ROOT_DIR"

MODE="${1:-gui}"

echo "=== Starting RatanaOS 64-bit in QEMU ($MODE mode) ==="

make all

case "$MODE" in
    gui)
        qemu-system-x86_64 -kernel build/ratanaos32.bin -vga std -serial stdio
        ;;
    iso)
        make iso
        if [ -f build/ratanaos.iso ]; then
            qemu-system-x86_64 -cdrom build/ratanaos.iso -vga std -serial stdio
        else
            echo "Notice: build/ratanaos.iso not found. Run 'sudo pacman -S xorriso' to generate ISO."
        fi
        ;;
    cli|terminal|serial)
        qemu-system-x86_64 -kernel build/ratanaos32.bin -display none -serial stdio
        ;;
    curses)
        qemu-system-x86_64 -kernel build/ratanaos32.bin -display curses || qemu-system-x86_64 -kernel build/ratanaos32.bin -display none -serial stdio
        ;;
    debug)
        qemu-system-x86_64 -kernel build/ratanaos32.bin -vga std -serial stdio -d int,cpu_reset -no-reboot
        ;;
    *)
        echo "Usage: $0 [gui | iso | cli | debug]"
        exit 1
        ;;
esac
