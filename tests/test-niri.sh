#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Test Suite: Niri Configuration Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
NIRI_DIR="${ROOT_DIR}/config/desktop/niri"

echo "==> Running Test: test-niri.sh..."

REQUIRED_FILES=(
    "config.kdl"
    "keybinds.kdl"
    "startup.kdl"
    "layout.kdl"
    "appearance.kdl"
)

for f in "${REQUIRED_FILES[@]}"; do
    FILE="${NIRI_DIR}/${f}"
    if [ ! -f "$FILE" ]; then
        echo "FAIL: Niri file '$FILE' is missing!" >&2
        exit 1
    fi
    if [ ! -s "$FILE" ]; then
        echo "FAIL: Niri file '$FILE' is empty!" >&2
        exit 1
    fi
    echo "  [PASS] Found: $f"
done

# Verify includes in config.kdl
for inc in layout appearance startup keybinds; do
    if ! grep -q "include \"${inc}.kdl\"" "${NIRI_DIR}/config.kdl"; then
        echo "FAIL: config.kdl does not include ${inc}.kdl!" >&2
        exit 1
    fi
done

echo "✅ All Niri tests PASSED."
