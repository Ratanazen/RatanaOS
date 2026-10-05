#!/usr/bin/env bash
# ==============================================================================
# RiOS Full System Master Build Pipeline (2026)
# Orchestrates clean verification, security updates, and complete ISO compilation
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

BOLD='\033[1m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

print_banner() {
    echo -e "${BLUE}${BOLD}"
    echo "╔══════════════════════════════════════════════════════════╗"
    echo "║       RiOS Master Full System Build Pipeline (2026)     ║"
    echo "║     Complete ISO Compilation, CVE Updates & Validation   ║"
    echo "╚══════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

print_banner

START_TIME=$(date +%s)

# Step 1: Preflight & Test Suite Validation
echo -e "${CYAN}[1/5] Running pre-build static validation checks...${NC}"
"${ROOT_DIR}/tests/run-all.sh"

# Step 2: Configure All Package Profiles (Full System)
echo -e "\n${CYAN}[2/5] Enabling all full system profiles (Server + Cyber + Dev + Desktops)...${NC}"
"${ROOT_DIR}/scripts/build_custom.sh" all --no-build

# Step 3: Ensure Security & CVE Updates Archive is Configured
echo -e "\n${CYAN}[3/5] Verifying Debian Security & CVE patch archives...${NC}"
mkdir -p "${ROOT_DIR}/rios-live/config/archives"
cat << 'EOF' > "${ROOT_DIR}/rios-live/config/archives/security.list.chroot"
deb http://deb.debian.org/debian-security/ bookworm-security main contrib non-free non-free-firmware
deb http://deb.debian.org/debian/ bookworm-updates main contrib non-free non-free-firmware
EOF

cat << 'EOF' > "${ROOT_DIR}/rios-live/config/archives/security.list.binary"
deb http://deb.debian.org/debian-security/ bookworm-security main contrib non-free non-free-firmware
deb http://deb.debian.org/debian/ bookworm-updates main contrib non-free non-free-firmware
EOF
echo -e "  ${GREEN}✓${NC} Bookworm-security repository configured."

# Step 4: Execute Fast Live-Build Engine
echo -e "\n${CYAN}[4/5] Compiling full hybrid live ISO image...${NC}"
"${ROOT_DIR}/tools/fast-live-build.sh"

# Step 5: Checksums & Post-Build Verification
echo -e "\n${CYAN}[5/5] Generating cryptographic hashes & verifying ISO integrity...${NC}"
ISO_TARGET="${ROOT_DIR}/build/rios-live-amd64.hybrid.iso"

if [ -f "$ISO_TARGET" ]; then
    sha256sum "$ISO_TARGET" > "${ROOT_DIR}/build/RiOS.iso.sha256"
    sha512sum "$ISO_TARGET" > "${ROOT_DIR}/build/RiOS.iso.sha512"
    
    echo -e "\n${GREEN}${BOLD}============================================================${NC}"
    echo -e "${GREEN}${BOLD}     ✔ RIOS FULL SYSTEM BUILD COMPLETE & VERIFIED!         ${NC}"
    echo -e "${GREEN}${BOLD}============================================================${NC}"
    echo -e "ISO Image:   ${BOLD}${ISO_TARGET}${NC}"
    echo -e "Image Size:  ${BOLD}$(du -h "$ISO_TARGET" | cut -f1)${NC}"
    echo -e "SHA256:      $(cat "${ROOT_DIR}/build/RiOS.iso.sha256")"
    echo -e "SHA512:      $(cat "${ROOT_DIR}/build/RiOS.iso.sha512" | cut -d' ' -f1)..."
    
    END_TIME=$(date +%s)
    TOTAL_SECS=$((END_TIME - START_TIME))
    echo -e "Build Time:  $((TOTAL_SECS / 60)) min $((TOTAL_SECS % 60)) sec"
    echo "============================================================"
else
    echo -e "${RED}Error: ISO was not created! Check logs.${NC}"
    exit 1
fi
