#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Test Suite: Filesystem Format & Mount Module Validation
# ==============================================================================
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FS_SCRIPT="${ROOT_DIR}/installer/filesystem.sh"

echo "==> Running Test: test-filesystem.sh..."

if [ ! -f "$FS_SCRIPT" ] || [ ! -x "$FS_SCRIPT" ]; then
    echo "FAIL: filesystem.sh missing or not executable!" >&2
    exit 1
fi

if ! bash -n "$FS_SCRIPT"; then
    echo "FAIL: Syntax error in filesystem.sh!" >&2
    exit 1
fi

source "$FS_SCRIPT"

for fn in format_filesystems mount_target unmount_target; do
    if ! declare -f "$fn" >/dev/null; then
        echo "FAIL: Function '$fn' not declared in filesystem.sh!" >&2
        exit 1
    fi
done

echo "PASS: test-filesystem.sh passed all checks!"
