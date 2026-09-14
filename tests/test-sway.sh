#!/usr/bin/env bash
# ==============================================================================
# RiOS Test Suite: Sway Configuration Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SWAY_CONF="${ROOT_DIR}/config/desktop/sway/config"

echo "==> Running Test: test-sway.sh..."

if [ ! -f "$SWAY_CONF" ]; then
    echo "FAIL: Sway configuration '$SWAY_CONF' missing!" >&2
    exit 1
fi

REQUIRED_DIRECTIVES=(
    "set \$mod Mod4"
    "bindsym \$mod+Return"
    "bindsym \$mod+space"
    "exec_always waybar"
    "exec swaync"
    "ri-theme apply"
)

for dir in "${REQUIRED_DIRECTIVES[@]}"; do
    if ! grep -Fq "$dir" "$SWAY_CONF"; then
        echo "FAIL: Missing directive '$dir' in Sway config!" >&2
        exit 1
    fi
    echo "  [PASS] Found: $dir"
done

echo "✅ All Sway tests PASSED."
