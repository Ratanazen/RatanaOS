#!/usr/bin/env bash
# ==============================================================================
# RiOS Test Suite: Waybar Layouts Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WAYBAR_DIR="${ROOT_DIR}/config/waybar"

echo "==> Running Test: test-waybar.sh..."

LAYOUTS=("config-top.json" "config-bottom.json" "config-minimal.json")

for layout in "${LAYOUTS[@]}"; do
    FILE="${WAYBAR_DIR}/${layout}"
    if [ ! -f "$FILE" ]; then
        echo "FAIL: Waybar layout '$FILE' missing!" >&2
        exit 1
    fi

    # Validate JSON syntax using python
    python3 -m json.tool "$FILE" > /dev/null
    echo "  [PASS] Valid JSON layout: $layout"
done

if [ ! -f "${WAYBAR_DIR}/style.css" ]; then
    echo "FAIL: Waybar style.css missing!" >&2
    exit 1
fi

echo "✅ All Waybar tests PASSED."
