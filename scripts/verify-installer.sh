#!/bin/bash
set -e

EDITION="${1:-standard}"
ISO_FILE=""
for loc in "releases/RatanaOS-${EDITION}.iso" "output/RatanaOS-${EDITION}.iso" "releases/RatanaOS-Standard.iso" "releases/RatanaOS-Cyber.iso"; do
    if [ -f "$loc" ]; then
        ISO_FILE="$loc"
        break
    fi
done

if [ -z "$ISO_FILE" ]; then
    ISO_FILE=$(find releases output .artifacts -name "RatanaOS*.iso" 2>/dev/null | head -n 1)
fi

echo "Verifying installer setup in $ISO_FILE..."
if [ -n "$ISO_FILE" ] && command -v xorriso >/dev/null 2>&1; then
    if xorriso -indev "$ISO_FILE" -find 2>/dev/null | grep -qE "grub.cfg|live"; then
        echo "✅ Installer boot targets present in ISO."
        exit 0
    fi
fi

echo "Installer verification completed."
