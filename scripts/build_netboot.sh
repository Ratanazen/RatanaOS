#!/usr/bin/env bash
# ==============================================================================
# RiOS Netboot & PXE Network Installation Asset Builder (2026)
# Extracts and packages kernel, initrd, squashfs, and iPXE/PXELINUX boot configs
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

ISO_PATH="${ROOT_DIR}/build/rios-live-amd64.hybrid.iso"
OUTPUT_DIR="${ROOT_DIR}/build/netboot"

print_banner() {
    echo -e "${BLUE}${BOLD}"
    echo "╔══════════════════════════════════════════════════════════╗"
    echo "║       RiOS Netboot & PXE Network Asset Builder           ║"
    echo "║       iPXE, PXELINUX, TFTP & HTTP Diskless Streaming     ║"
    echo "╚══════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

print_banner

if [ ! -f "$ISO_PATH" ]; then
    echo -e "${RED}Error: ISO file not found at: ${ISO_PATH}${NC}"
    echo "Please build the ISO first using './build.sh full'."
    exit 1
fi

echo -e "${CYAN}[1/4] Preparing Netboot output directory: ${OUTPUT_DIR}...${NC}"
mkdir -p "${OUTPUT_DIR}/pxelinux.cfg"
mkdir -p "${OUTPUT_DIR}/assets"

echo -e "${CYAN}[2/4] Extracting kernel, initramfs, and squashfs from ISO using 7z/bsdtar...${NC}"
TMP_EXTRACT=$(mktemp -d)
cleanup() {
    rm -rf "$TMP_EXTRACT" 2>/dev/null || true
}
trap cleanup EXIT

if command -v 7z >/dev/null 2>&1; then
    7z x -y "$ISO_PATH" "live/vmlinuz*" "live/initrd.img*" "live/filesystem.squashfs" -o"$TMP_EXTRACT" >/dev/null
elif command -v bsdtar >/dev/null 2>&1; then
    bsdtar -xf "$ISO_PATH" -C "$TMP_EXTRACT" "live/vmlinuz" "live/initrd.img" "live/filesystem.squashfs"
else
    echo -e "${RED}Error: Neither 7z nor bsdtar installed to extract ISO assets.${NC}"
    exit 1
fi

echo -e "${CYAN}[3/4] Organizing assets into ${OUTPUT_DIR}/assets...${NC}"
cp -f "$TMP_EXTRACT"/live/vmlinuz* "${OUTPUT_DIR}/assets/vmlinuz" 2>/dev/null || cp -f "$TMP_EXTRACT"/live/vmlinuz "${OUTPUT_DIR}/assets/vmlinuz"
cp -f "$TMP_EXTRACT"/live/initrd.img* "${OUTPUT_DIR}/assets/initrd.img" 2>/dev/null || cp -f "$TMP_EXTRACT"/live/initrd.img "${OUTPUT_DIR}/assets/initrd.img"
cp -f "$TMP_EXTRACT"/live/filesystem.squashfs "${OUTPUT_DIR}/assets/filesystem.squashfs"

echo -e "${CYAN}[4/4] Generating iPXE and PXELINUX configuration scripts...${NC}"

# Generate iPXE boot script
cat << 'IPXEEOF' > "${OUTPUT_DIR}/boot.ipxe"
#!ipxe
# RiOS 2026 iPXE Network Boot Script
echo Booting RiOS 2026 over Network (HTTP/TFTP)...
set server_ip ${next-server}
kernel http://${server_ip}:8080/assets/vmlinuz boot=live components fetch=http://${server_ip}:8080/assets/filesystem.squashfs quiet splash
initrd http://${server_ip}:8080/assets/initrd.img
boot
IPXEEOF

# Generate PXELINUX config
cat << 'PXEEOF' > "${OUTPUT_DIR}/pxelinux.cfg/default"
DEFAULT rios
PROMPT 0
TIMEOUT 50

LABEL rios
  MENU LABEL RiOS 2026 (Network Boot)
  KERNEL assets/vmlinuz
  APPEND initrd=assets/initrd.img boot=live components fetch=http://192.168.1.1:8080/assets/filesystem.squashfs quiet splash
PXEEOF

# Generate local HTTP server launcher
cat << 'SERVEOF' > "${OUTPUT_DIR}/serve.sh"
#!/usr/bin/env bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PORT=8080
echo "Starting RiOS Netboot Asset HTTP Server on port ${PORT}..."
echo "Serving assets for iPXE and PXE clients:"
echo "  - http://<HOST-IP>:${PORT}/boot.ipxe"
echo "  - http://<HOST-IP>:${PORT}/assets/vmlinuz"
echo "  - http://<HOST-IP>:${PORT}/assets/initrd.img"
echo "  - http://<HOST-IP>:${PORT}/assets/filesystem.squashfs"
echo ""
cd "$SCRIPT_DIR"
python3 -m http.server ${PORT}
SERVEOF
chmod +x "${OUTPUT_DIR}/serve.sh"

echo -e "\n${GREEN}${BOLD}============================================================${NC}"
echo -e "${GREEN}${BOLD}      ✔ RIOS NETBOOT & PXE ASSETS COMPILED SUCCESSFULLY!    ${NC}"
echo -e "${GREEN}${BOLD}============================================================${NC}"
echo -e "Netboot Root:    ${BOLD}${OUTPUT_DIR}${NC}"
echo -e "Kernel Asset:    ${OUTPUT_DIR}/assets/vmlinuz ($(du -h "${OUTPUT_DIR}/assets/vmlinuz" | cut -f1))"
echo -e "Initrd Asset:    ${OUTPUT_DIR}/assets/initrd.img ($(du -h "${OUTPUT_DIR}/assets/initrd.img" | cut -f1))"
echo -e "SquashFS:        ${OUTPUT_DIR}/assets/filesystem.squashfs ($(du -h "${OUTPUT_DIR}/assets/filesystem.squashfs" | cut -f1))"
echo -e "iPXE Config:     ${OUTPUT_DIR}/boot.ipxe"
echo -e "PXELINUX Config: ${OUTPUT_DIR}/pxelinux.cfg/default"
echo -e "\nTo serve these assets over LAN, run:"
echo -e "  ${BOLD}${OUTPUT_DIR}/serve.sh${NC}"
echo "============================================================"
echo ""
