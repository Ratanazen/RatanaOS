#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Test Suite: ISO Structure & Artifact Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
ISO_FILE="${BUILD_DIR}/ratanaos-live-amd64.hybrid.iso"

echo "==> Running Test: test-iso.sh..."

if [ ! -f "$ISO_FILE" ]; then
    echo "FAIL: ISO file '$ISO_FILE' does not exist!" >&2
    exit 1
fi

SIZE=$(stat -c%s "$ISO_FILE" 2>/dev/null || stat -f%z "$ISO_FILE")
SIZE_GB=$(awk "BEGIN {printf \"%.2f\", $SIZE/1073741824}")

echo "  [PASS] ISO File exists: $ISO_FILE"
echo "  [PASS] ISO Size: ${SIZE_GB} GB (${SIZE} bytes)"

# Minimum acceptable size is 2.5GB for a full multi-desktop OS
if [ "$SIZE" -lt 2684354560 ]; then
    echo "FAIL: ISO size is under 2.5 GB! Possible incomplete build." >&2
    exit 1
fi

if [ -f "${BUILD_DIR}/RatanaOS.iso.sha256" ]; then
    echo "  [PASS] Checksum file exists: $(cat "${BUILD_DIR}/RatanaOS.iso.sha256")"
fi

echo "✅ All ISO structure tests PASSED."
