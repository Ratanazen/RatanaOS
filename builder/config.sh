#!/bin/bash
set -e

EDITION=${1:-standard}

echo "Configuring for edition: $EDITION"

# Remove old generated configs so live-build is forced to use our new Debian mode
rm -f config/bootstrap config/chroot config/binary config/common config/source

lb config noauto \
    --mode debian \
    --architectures amd64 \
    --linux-flavours amd64 \
    --distribution trixie \
    --mirror-bootstrap "http://deb.debian.org/debian/" \
    --mirror-chroot "http://deb.debian.org/debian/" \
    --mirror-binary "http://deb.debian.org/debian/" \
    --archive-areas "main contrib non-free non-free-firmware" \
    --bootappend-live "boot=live components quiet splash findiso=\${iso_path}" \
    --binary-images iso-hybrid \
    --iso-application "RatanaOS Live" \
    --iso-publisher "RatanaOS Project" \
    --iso-volume "RATANAOS_$EDITION" \
    --win32-loader false \
    --memtest none \
    --apt-indices none \
    --apt-recommends false \
    --compression squashfs \
    --system live \
    "${@}"

# Setup includes
mkdir -p config/includes.chroot/usr/share/themes
mkdir -p config/includes.chroot/usr/share/icons
mkdir -p config/includes.chroot/usr/share/backgrounds/ratana
cp -r themes/* config/includes.chroot/usr/share/themes/ 2>/dev/null || true
cp -r branding/icons/* config/includes.chroot/usr/share/icons/ 2>/dev/null || true
cp -r branding/wallpapers/* config/includes.chroot/usr/share/backgrounds/ratana/ 2>/dev/null || true

echo "Configuration completed."
