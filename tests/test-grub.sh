#!/usr/bin/env bash
# ==============================================================================
# RiOS Test Suite: Bootloader & GRUB Configuration Validation
# ==============================================================================
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BOOT_SCRIPT="${ROOT_DIR}/installer/bootloader.sh"

echo "==> Running Test: test-grub.sh..."

if [ ! -f "$BOOT_SCRIPT" ] || [ ! -x "$BOOT_SCRIPT" ]; then
    echo "FAIL: bootloader.sh missing or not executable!" >&2
    exit 1
fi

if ! bash -n "$BOOT_SCRIPT"; then
    echo "FAIL: Syntax error in bootloader.sh!" >&2
    exit 1
fi

if ! grep -q "RiOS" "$BOOT_SCRIPT" || ! grep -q "grub-install" "$BOOT_SCRIPT"; then
    echo "FAIL: bootloader.sh does not reference RiOS or grub-install!" >&2
    exit 1
fi

echo "PASS: test-grub.sh passed all checks!"
