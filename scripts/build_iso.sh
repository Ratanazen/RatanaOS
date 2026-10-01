#!/usr/bin/env bash
# ==============================================================================
# RiOS ISO Build Script Wrapper
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

echo "=== Building RiOS 64-bit Bootable Hybrid Live ISO ==="
exec "${ROOT_DIR}/build.sh" "$@"
