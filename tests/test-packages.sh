#!/usr/bin/env bash
# ==============================================================================
# RiOS Test Suite: Package Lists Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PKG_DIR="${ROOT_DIR}/packages"

echo "==> Running Test: test-packages.sh..."

REQUIRED_LISTS=("cyber.list" "dev.list" "server.list" "gnome.list" "kde.list" "hyprland.list" "sway.list" "desktop.list" "common.list" "installer.list" "fonts.list")

for list in "${REQUIRED_LISTS[@]}"; do
    FILE="${PKG_DIR}/${list}"
    if [ ! -f "$FILE" ]; then
        echo "FAIL: Package list '$FILE' missing!" >&2
        exit 1
    fi
    # Ensure at least 3 packages declared
    COUNT=$(grep -v "^#" "$FILE" | grep -v "^$" | wc -l)
    if [ "$COUNT" -lt 3 ]; then
        echo "FAIL: Package list '$FILE' has fewer than 3 packages ($COUNT)!" >&2
        exit 1
    fi
    echo "  [PASS] List: $list ($COUNT packages declared)"
done

echo "✅ All Package List tests PASSED."

