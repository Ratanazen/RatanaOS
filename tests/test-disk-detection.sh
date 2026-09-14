#!/usr/bin/env bash
# ==============================================================================
# RiOS Test Suite: Safe Disk Detection Validation
# ==============================================================================
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DISK_SCRIPT="${ROOT_DIR}/installer/disk.sh"

echo "==> Running Test: test-disk-detection.sh..."

if [ ! -f "$DISK_SCRIPT" ] || [ ! -x "$DISK_SCRIPT" ]; then
    echo "FAIL: disk.sh missing or not executable!" >&2
    exit 1
fi

if ! bash -n "$DISK_SCRIPT"; then
    echo "FAIL: Syntax error in disk.sh!" >&2
    exit 1
fi

source "$DISK_SCRIPT"

# Verify critical functions exist
for fn in get_live_device list_disks show_disk_details is_safe_disk; do
    if ! declare -f "$fn" >/dev/null; then
        echo "FAIL: Function '$fn' not declared in disk.sh!" >&2
        exit 1
    fi
done

# Safety checks
if is_safe_disk "/dev/null"; then
    echo "FAIL: is_safe_disk accepted /dev/null which is not a block device!" >&2
    exit 1
fi

if is_safe_disk "/dev/nonexistent_disk_xyz"; then
    echo "FAIL: is_safe_disk accepted nonexistent device!" >&2
    exit 1
fi

echo "PASS: test-disk-detection.sh passed all checks!"
