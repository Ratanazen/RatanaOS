#!/usr/bin/env bash
# ==============================================================================
# RiOS Test Suite: 4-Edition Architecture & System 2026 Validation
# Validates: All/Full, Server, Cyber, and Dev editions
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_SCRIPT="${ROOT_DIR}/build.sh"
CUSTOM_SCRIPT="${ROOT_DIR}/scripts/build_custom.sh"
CHROOT_DIR="${ROOT_DIR}/rios-live/config/includes.chroot"
PKG_DIR="${ROOT_DIR}/packages"

echo "==> Running Test: test-editions.sh..."

# 1. Verify build script and custom builder exist and are executable
if [ ! -x "$BUILD_SCRIPT" ] || [ ! -x "$CUSTOM_SCRIPT" ]; then
    echo "FAIL: Build script or custom builder missing or not executable!" >&2
    exit 1
fi
echo "  [PASS] Build scripts executable"

# 2. Verify all 4 editions are supported in build.sh
EDITIONS=("all" "server" "cyber" "dev" "all4")
for ed in "${EDITIONS[@]}"; do
    if ! grep -q "${ed}" "$BUILD_SCRIPT"; then
        echo "FAIL: Edition '${ed}' not declared in build.sh!" >&2
        exit 1
    fi
    echo "  [PASS] Declared edition in build.sh: $ed"
done

# 3. Test multi-edition batch runner
OUTPUT=$("${BUILD_SCRIPT}" all4)
if ! echo "$OUTPUT" | grep -q "All 4 Editions Successfully Configured"; then
    echo "FAIL: ./build.sh all4 did not report successful validation!" >&2
    exit 1
fi
echo "  [PASS] Multi-edition orchestrator (all4) passed"

# 4. Verify core package lists for the 4 editions
CORE_LISTS=("server.list" "cyber.list" "dev.list" "desktop.list" "wayland.list")
for l in "${CORE_LISTS[@]}"; do
    if [ ! -f "${PKG_DIR}/${l}" ]; then
        echo "FAIL: Core edition package list '${PKG_DIR}/${l}' missing!" >&2
        exit 1
    fi
    COUNT=$(grep -v "^#" "${PKG_DIR}/${l}" | grep -v "^$" | wc -l)
    if [ "$COUNT" -lt 5 ]; then
        echo "FAIL: Package list '${l}' has fewer than 5 packages ($COUNT)!" >&2
        exit 1
    fi
    echo "  [PASS] Package suite: $l ($COUNT packages)"
done

# 5. Verify 2026 system identity
OS_RELEASE="${CHROOT_DIR}/etc/os-release"
if [ ! -f "$OS_RELEASE" ]; then
    echo "FAIL: os-release missing from chroot!" >&2
    exit 1
fi

if ! grep -q "2026" "$OS_RELEASE"; then
    echo "FAIL: os-release does not reflect 2026 release!" >&2
    exit 1
fi
echo "  [PASS] OS Release identity: $(grep PRETTY_NAME "$OS_RELEASE" | cut -d= -f2)"

# 6. Verify MOTD & Issue
if ! grep -q "2026" "${CHROOT_DIR}/etc/issue"; then
    echo "FAIL: /etc/issue does not reflect 2026 release!" >&2
    exit 1
fi
if ! grep -q "2026" "${CHROOT_DIR}/etc/motd"; then
    echo "FAIL: /etc/motd does not reflect 2026 release!" >&2
    exit 1
fi
echo "  [PASS] /etc/issue and /etc/motd 2026 identity verified"

echo "PASS: test-editions.sh passed all checks!"
