#!/bin/bash
set -e

EDITION=${1:-standard}
ISO_FILE="output/RatanaOS-${EDITION}.iso"
REPORT_FILE="output/ISO_VALIDATION.md"

echo "Validating $ISO_FILE..."

if [ ! -f "$ISO_FILE" ]; then
    echo "Error: $ISO_FILE does not exist!"
    exit 1
fi

SIZE_MB=$(du -m "$ISO_FILE" | cut -f1)
echo "ISO Size: ${SIZE_MB} MB"

if [ "$SIZE_MB" -lt 500 ]; then
    echo "Error: ISO size is below 500 MB. Packages were likely not installed."
    exit 1
fi

# Mount and check internals
MNT_DIR=$(mktemp -d)
sudo mount -o loop "$ISO_FILE" "$MNT_DIR"

echo "# ISO Validation Report" > "$REPORT_FILE"
echo "Validation Date: $(date)" >> "$REPORT_FILE"
echo "- ISO Exists: ✅" >> "$REPORT_FILE"
echo "- ISO Size: ${SIZE_MB} MB ✅" >> "$REPORT_FILE"

if [ -f "$MNT_DIR/live/filesystem.squashfs" ]; then
    echo "- filesystem.squashfs exists: ✅" >> "$REPORT_FILE"
else
    echo "- filesystem.squashfs exists: ❌" >> "$REPORT_FILE"
    exit 1
fi

if [ -f "$MNT_DIR/live/vmlinuz" ] || ls "$MNT_DIR/live"/vmlinuz* 1> /dev/null 2>&1; then
    echo "- vmlinuz exists: ✅" >> "$REPORT_FILE"
else
    echo "- vmlinuz exists: ❌" >> "$REPORT_FILE"
    exit 1
fi

if [ -f "$MNT_DIR/live/initrd.img" ] || ls "$MNT_DIR/live"/initrd* 1> /dev/null 2>&1; then
    echo "- initrd exists: ✅" >> "$REPORT_FILE"
else
    echo "- initrd exists: ❌" >> "$REPORT_FILE"
    exit 1
fi

if [ -f "$MNT_DIR/boot/grub/grub.cfg" ] || [ -f "$MNT_DIR/EFI/BOOT/grub.cfg" ] || [ -f "$MNT_DIR/boot/grub/loopback.cfg" ]; then
    echo "- grub.cfg exists: ✅" >> "$REPORT_FILE"
else
    echo "- grub.cfg exists: ❌" >> "$REPORT_FILE"
    exit 1
fi

if [ -d "$MNT_DIR/EFI" ]; then
    echo "- EFI boot files exist: ✅" >> "$REPORT_FILE"
else
    echo "- EFI boot files exist: ❌" >> "$REPORT_FILE"
    exit 1
fi

sudo umount "$MNT_DIR"
rmdir "$MNT_DIR"

echo "Validation successful!"
