#!/usr/bin/env bash
# ==============================================================================
# RiOS Test Suite: Wayland Sessions & Desktop Services Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SESSIONS_DIR="${ROOT_DIR}/rios-live/config/includes.chroot/usr/share/wayland-sessions"

echo "==> Running Test: test-services.sh..."

if [ ! -d "$SESSIONS_DIR" ]; then
    echo "FAIL: Wayland sessions directory '$SESSIONS_DIR' missing!" >&2
    exit 1
fi

REQUIRED_SESSIONS=("rios-hyprland.desktop" "rios-sway.desktop")

for session in "${REQUIRED_SESSIONS[@]}"; do
    FILE="${SESSIONS_DIR}/${session}"
    if [ ! -f "$FILE" ]; then
        echo "FAIL: Desktop session file '$FILE' missing!" >&2
        exit 1
    fi
    if ! grep -q "Exec=" "$FILE" || ! grep -q "Type=Application" "$FILE"; then
        echo "FAIL: Desktop session file '$FILE' invalid desktop entry format!" >&2
        exit 1
    fi
done

# Verify helper tools are present and executable in scripts/
REQUIRED_SCRIPTS=(
    "ri-launcher"
    "ri-lock"
    "ri-power"
    "ri-screenshot"
    "ri-performance"
    "ri-monitor"
    "ri-first-run"
    "ri-doctor"
)

for s in "${REQUIRED_SCRIPTS[@]}"; do
    SCRIPT_PATH="${ROOT_DIR}/scripts/${s}"
    if [ ! -f "$SCRIPT_PATH" ]; then
        echo "FAIL: Required script '$SCRIPT_PATH' is missing!" >&2
        exit 1
    fi
    if [ ! -x "$SCRIPT_PATH" ]; then
        echo "FAIL: Script '$SCRIPT_PATH' is not executable!" >&2
        exit 1
    fi
    if ! bash -n "$SCRIPT_PATH"; then
        echo "FAIL: Script '$SCRIPT_PATH' has syntax errors!" >&2
        exit 1
    fi
done

echo "PASS: test-services.sh successfully passed all checks!"
