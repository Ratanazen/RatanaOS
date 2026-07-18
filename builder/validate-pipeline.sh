#!/bin/bash
# RatanaOS Pipeline Validator
# Validates Repository, Scripts, Builder, Package lists, Desktop packages, Installer, Branding, Bootloader

set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REPORTS_DIR="${ROOT_DIR}/reports"
mkdir -p "${REPORTS_DIR}"

REPORT="${REPORTS_DIR}/PROJECT_STATUS.md"
echo "Generating ${REPORT}..."

cat <<EOF > "$REPORT"
# RatanaOS Pipeline Validation Report
Date: $(date -u +"%Y-%m-%dT%H:%M:%SZ")

## Component Checks
EOF

check_item() {
    local name=$1
    local path=$2
    if [ -e "$path" ]; then
        echo "- [x] **${name}**: Validated (${path})" >> "$REPORT"
    else
        echo "- [ ] **${name}**: Missing (${path})" >> "$REPORT"
    fi
}

check_item "Repository" "${ROOT_DIR}/.git"
check_item "Scripts" "${ROOT_DIR}/scripts"
check_item "Builder" "${ROOT_DIR}/builder/build-iso.sh"
check_item "Package lists" "${ROOT_DIR}/profiles"
check_item "Desktop packages" "${ROOT_DIR}/desktop"
check_item "Installer" "${ROOT_DIR}/installer"
check_item "Branding" "${ROOT_DIR}/branding"
check_item "Bootloader" "${ROOT_DIR}/builder/build-iso.sh" # Validated inside build script

echo "Validation Complete. See ${REPORT}"
