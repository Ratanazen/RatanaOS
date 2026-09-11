#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Test Suite: fstab Generation Logic Validation
# ==============================================================================
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FINISH_SCRIPT="${ROOT_DIR}/installer/finish.sh"

echo "==> Running Test: test-fstab.sh..."

if [ ! -f "$FINISH_SCRIPT" ] || [ ! -x "$FINISH_SCRIPT" ]; then
    echo "FAIL: finish.sh missing or not executable!" >&2
    exit 1
fi

if ! bash -n "$FINISH_SCRIPT"; then
    echo "FAIL: Syntax error in finish.sh!" >&2
    exit 1
fi

if ! grep -q "UUID=" "$FINISH_SCRIPT" || ! grep -q "tmpfs" "$FINISH_SCRIPT"; then
    echo "FAIL: finish.sh does not generate UUID-based fstab entries!" >&2
    exit 1
fi

echo "PASS: test-fstab.sh passed all checks!"
