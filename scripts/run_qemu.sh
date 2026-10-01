#!/usr/bin/env bash
# ==============================================================================
# RiOS QEMU Unified Virtual Machine Runner (1.0.0)
# Supports: Desktop, Server, CLI/Headless, and Installed Disk Testing
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

MODE="${1:-desktop}"
ISO_PATH="${ROOT_DIR}/build/rios-live-amd64.hybrid.iso"
DISK_PATH="/tmp/rios-target-disk.qcow2"

case "$MODE" in
    desktop|gui)
        echo "=== Launching RiOS Desktop Environment in QEMU ==="
        qemu-system-x86_64 \
            -enable-kvm -cpu host \
            -m 4096 -smp 2 \
            -cdrom "${ISO_PATH}" \
            -vga virtio -display default \
            -device intel-hda -device hda-duplex \
            -net nic -net user
        ;;
    server)
        echo "=== Launching RiOS Server Mode ==="
        exec "${SCRIPT_DIR}/run_server.sh" --curses
        ;;
    headless|cli)
        echo "=== Launching RiOS Headless Server ==="
        exec "${SCRIPT_DIR}/run_server.sh" --headless
        ;;
    disk)
        echo "=== Launching RiOS Installed Virtual Disk ==="
        if [ ! -f "${DISK_PATH}" ]; then
            echo "Error: Installed disk ${DISK_PATH} not found. Run installer test first."
            exit 1
        fi
        qemu-system-x86_64 \
            -enable-kvm -cpu host \
            -m 4096 -smp 2 \
            -drive file="${DISK_PATH}",format=qcow2,if=virtio \
            -vga virtio -display default \
            -net nic -net user
        ;;
    *)
        echo "Usage: $0 [desktop | server | cli | headless | disk]"
        exit 1
        ;;
esac
