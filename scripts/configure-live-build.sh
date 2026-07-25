#!/bin/bash
set -e
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "Configuring Debian live-build..."
if [ -x "${ROOT_DIR}/auto/config" ]; then
    "${ROOT_DIR}/auto/config" "${@}"
else
    lb config \
        --mode debian \
        --distribution bookworm \
        --binary-images iso-hybrid \
        --architectures amd64 \
        --linux-flavours amd64 \
        --archive-areas "main contrib non-free non-free-firmware" \
        --apt-indices false \
        --bootloader grub-pc \
        --memtest none \
        "${@}"
fi
echo "Live build configuration complete."
