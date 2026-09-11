#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Test Suite: First-Boot Systemd & User Provisioning Validation
# ==============================================================================
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SERVICE_FILE="${ROOT_DIR}/ratanaos-live/config/includes.chroot/etc/systemd/system/ratana-first-boot.service"
SCRIPT_FILE="${ROOT_DIR}/scripts/ratana-first-boot"
USER_FIRST_RUN="${ROOT_DIR}/scripts/ratana-first-run"

echo "==> Running Test: test-first-boot.sh..."

if [ ! -f "$SERVICE_FILE" ]; then
    echo "FAIL: Service file '$SERVICE_FILE' missing!" >&2
    exit 1
fi

if [ ! -f "$SCRIPT_FILE" ] || [ ! -x "$SCRIPT_FILE" ]; then
    echo "FAIL: Script '$SCRIPT_FILE' missing or not executable!" >&2
    exit 1
fi

if ! grep -q "ExecStart=/usr/local/bin/ratana-first-boot" "$SERVICE_FILE"; then
    echo "FAIL: Service file does not call ratana-first-boot!" >&2
    exit 1
fi

if ! grep -q ".first-boot-done" "$SERVICE_FILE"; then
    echo "FAIL: Service file does not declare ConditionPathExists on first-boot marker!" >&2
    exit 1
fi

if ! grep -q ".first-run-complete" "$USER_FIRST_RUN"; then
    echo "FAIL: ratana-first-run does not check or touch .first-run-complete!" >&2
    exit 1
fi

echo "PASS: test-first-boot.sh passed all checks!"
