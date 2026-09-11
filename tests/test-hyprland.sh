#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Test Suite: Hyprland Modular Configuration Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
HYPR_DIR="${ROOT_DIR}/config/desktop/hyprland"

echo "==> Running Test: test-hyprland.sh..."

REQUIRED_FILES=(
    "hyprland.conf"
    "keybinds.conf"
    "animations.conf"
    "rules.conf"
    "monitors.conf"
    "environment.conf"
    "startup.conf"
)

for f in "${REQUIRED_FILES[@]}"; do
    FILE="${HYPR_DIR}/${f}"
    if [ ! -f "$FILE" ]; then
        echo "FAIL: Hyprland configuration '$FILE' is missing!" >&2
        exit 1
    fi
    # Check syntax isn't empty
    if [ ! -s "$FILE" ]; then
        echo "FAIL: Hyprland configuration '$FILE' is empty!" >&2
        exit 1
    fi
    echo "  [PASS] Found: $f"
done

# Verify sources in hyprland.conf
for mod in monitors environment startup animations rules keybinds; do
    if ! grep -q "source = ~/.config/hypr/${mod}.conf" "${HYPR_DIR}/hyprland.conf"; then
        echo "FAIL: hyprland.conf does not source ~/.config/hypr/${mod}.conf!" >&2
        exit 1
    fi
done

echo "✅ All Hyprland tests PASSED."
