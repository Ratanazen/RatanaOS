#!/usr/bin/env bash
# ==============================================================================
# RiOS Test Suite: Font Definitions & Configuration Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PKG_LIST="${ROOT_DIR}/rios-live/config/package-lists/rios-desktop.list.chroot"
PKG_DESK="${ROOT_DIR}/packages/desktop.list"

echo "==> Running Test: test-fonts.sh..."

REQUIRED_FONTS=("fonts-dejavu-core" "fonts-noto-core" "fonts-font-awesome" "fonts-khmeros")

for font in "${REQUIRED_FONTS[@]}"; do
    FOUND=0
    if [ -f "$PKG_LIST" ] && grep -q "$font" "$PKG_LIST"; then
        FOUND=1
    fi
    if [ -f "$PKG_DESK" ] && grep -q "$font" "$PKG_DESK"; then
        FOUND=1
    fi
    if [ "$FOUND" -eq 0 ]; then
        echo "FAIL: Required font package '$font' not declared in package lists!" >&2
        exit 1
    fi
done

# Verify Waybar and Kitty reference valid font families
WAYBAR_CSS="${ROOT_DIR}/config/waybar/style.css"
if [ -f "$WAYBAR_CSS" ]; then
    if ! grep -qi "font-family" "$WAYBAR_CSS"; then
        echo "FAIL: Waybar CSS does not specify font-family!" >&2
        exit 1
    fi
fi

KITTY_CONF="${ROOT_DIR}/config/kitty/kitty.conf"
if [ -f "$KITTY_CONF" ]; then
    if ! grep -q "font_family" "$KITTY_CONF"; then
        echo "FAIL: Kitty config does not specify font_family!" >&2
        exit 1
    fi
fi

echo "PASS: test-fonts.sh successfully passed all checks!"
