#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Test Suite: Theme Engine Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
THEMES_DIR="${ROOT_DIR}/themes"

echo "==> Running Test: test-theme.sh..."

REQUIRED_THEMES=("ratana-dark" "ratana-light" "ratana-glass" "ratana-cyber" "ratana-anime" "ratana-minimal" "ratana-purple")
REQUIRED_VARS=("RATANA_THEME" "RATANA_ACCENT" "RATANA_BACKGROUND" "RATANA_FOREGROUND" "RATANA_RADIUS" "RATANA_GAP" "RATANA_BLUR" "RATANA_WALLPAPER")

for theme in "${REQUIRED_THEMES[@]}"; do
    FILE="${THEMES_DIR}/${theme}/theme.conf"
    if [ ! -f "$FILE" ]; then
        echo "FAIL: Theme file '$FILE' is missing!" >&2
        exit 1
    fi

    # Check that all required variables are set
    for var in "${REQUIRED_VARS[@]}"; do
        if ! grep -q "^${var}=" "$FILE"; then
            echo "FAIL: Variable '$var' missing in '$FILE'!" >&2
            exit 1
        fi
    done
    echo "  [PASS] Theme: $theme"
done

# Verify theme switcher script exists and is executable
if [ ! -x "${ROOT_DIR}/scripts/ratana-theme" ]; then
    echo "FAIL: ${ROOT_DIR}/scripts/ratana-theme is missing or not executable!" >&2
    exit 1
fi

echo "✅ All Theme Engine tests PASSED."
