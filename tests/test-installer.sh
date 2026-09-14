#!/usr/bin/env bash
# ==============================================================================
# RiOS Test Suite: Universal Desktop Installer Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
INSTALLER="${ROOT_DIR}/installer/ri-desktop-install.sh"

echo "==> Running Test: test-installer.sh..."

if [ ! -f "$INSTALLER" ]; then
    echo "FAIL: Installer '$INSTALLER' does not exist!" >&2
    exit 1
fi

if [ ! -x "$INSTALLER" ]; then
    echo "FAIL: Installer '$INSTALLER' is not executable!" >&2
    exit 1
fi

# 1. Syntax check
if ! bash -n "$INSTALLER"; then
    echo "FAIL: Installer syntax error detected by bash -n!" >&2
    exit 1
fi

# 2. Check for critical functions and flags
REQUIRED_PATTERNS=(
    "detect_system"
    "install_packages"
    "install_themes"
    "install_configs"
    "verify_installation"
    "--dry-run"
    "--profile"
    "--theme"
)

for pat in "${REQUIRED_PATTERNS[@]}"; do
    if ! grep -q -- "$pat" "$INSTALLER"; then
        echo "FAIL: Installer missing required pattern or function '$pat'!" >&2
        exit 1
    fi
done

# 3. Test dry-run execution
DRY_OUTPUT=$("$INSTALLER" --dry-run --profile hyprland --theme ri-dark 2>&1 || true)
if ! echo "$DRY_OUTPUT" | grep -qi "Dry run enabled"; then
    echo "FAIL: Installer dry run did not indicate dry run mode!" >&2
    exit 1
fi

echo "PASS: test-installer.sh successfully passed all checks!"
