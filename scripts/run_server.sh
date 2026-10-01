#!/usr/bin/env bash
# ==============================================================================
# RiOS Server Virtual Machine Runner (QEMU / KVM)
# Launches RiOS configured for Server & Cloud workloads with port forwarding:
#   - SSH:      localhost:2222  -> guest:22
#   - HTTP:     localhost:8080  -> guest:80
#   - HTTPS:    localhost:8443  -> guest:443
#   - Cockpit:  localhost:9090  -> guest:9090
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

BOLD='\033[1m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'

ISO_PATH="${ROOT_DIR}/build/rios-live-amd64.hybrid.iso"
DISK_PATH="/tmp/rios-target-disk.qcow2"
BOOT_SRC="iso"
DISPLAY_MODE="curses"
RAM_MB=4096
CPU_CORES=2
ENABLE_KVM=1

print_banner() {
    echo -e "${BLUE}${BOLD}"
    echo "╔══════════════════════════════════════════════════════════╗"
    echo "║          RiOS Virtual Server Launcher (QEMU)             ║"
    echo "║       High-Performance Server Environment with KVM       ║"
    echo "╚══════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

usage() {
    print_banner
    echo -e "${BOLD}Usage:${NC} $0 [OPTIONS]"
    echo ""
    echo -e "${BOLD}Options:${NC}"
    echo "  --iso [path]         Boot from Live ISO (default: build/rios-live-amd64.hybrid.iso)"
    echo "  --disk [path]        Boot from installed QCOW2/RAW disk (default: /tmp/rios-target-disk.qcow2)"
    echo "  --headless           Run in background / headless without display window"
    echo "  --gui                Run with graphical VGA display window"
    echo "  --curses             Run in terminal text mode (default)"
    echo "  --ram <MB>           Allocated RAM in MB (default: 4096)"
    echo "  --cores <N>          Allocated CPU cores (default: 2)"
    echo "  --no-kvm             Disable KVM hardware acceleration"
    echo "  -h, --help           Show this help"
    echo ""
    echo -e "${BOLD}Networking & Port Forwards:${NC}"
    echo "  SSH:     ssh -p 2222 ri@localhost  (or root@localhost)"
    echo "  HTTP:    http://localhost:8080"
    echo "  HTTPS:   https://localhost:8443"
    echo "  Cockpit: http://localhost:9090"
    echo ""
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --iso)
            BOOT_SRC="iso"
            if [[ -n "${2:-}" && ! "$2" =~ ^-- ]]; then
                ISO_PATH="$2"
                shift
            fi
            shift
            ;;
        --disk)
            BOOT_SRC="disk"
            if [[ -n "${2:-}" && ! "$2" =~ ^-- ]]; then
                DISK_PATH="$2"
                shift
            fi
            shift
            ;;
        --headless|--none)
            DISPLAY_MODE="none"
            shift
            ;;
        --gui|--vga)
            DISPLAY_MODE="gui"
            shift
            ;;
        --curses)
            DISPLAY_MODE="curses"
            shift
            ;;
        --ram)
            RAM_MB="$2"
            shift 2
            ;;
        --cores)
            CPU_CORES="$2"
            shift 2
            ;;
        --no-kvm)
            ENABLE_KVM=0
            shift
            ;;
        -h|--help)
            usage
            ;;
        *)
            echo "Unknown option: $1"
            usage
            ;;
    esac
done

print_banner

# Check QEMU binary
if ! command -v qemu-system-x86_64 >/dev/null 2>&1; then
    echo -e "${RED}Error: qemu-system-x86_64 is not installed!${NC}" >&2
    exit 1
fi

# Build QEMU arguments
QEMU_ARGS=(
    "-m" "${RAM_MB}"
    "-smp" "${CPU_CORES}"
    "-netdev" "user,id=net0,hostfwd=tcp::2222-:22,hostfwd=tcp::8080-:80,hostfwd=tcp::8443-:443,hostfwd=tcp::9090-:9090"
    "-device" "virtio-net-pci,netdev=net0"
)

# KVM Acceleration
if [ "$ENABLE_KVM" -eq 1 ] && [ -w /dev/kvm ]; then
    echo -e "  ${GREEN}✓${NC} KVM Hardware Acceleration enabled"
    QEMU_ARGS+=("-enable-kvm" "-cpu" "host")
else
    echo -e "  ${YELLOW}!${NC} KVM not available, using TCG emulation"
    QEMU_ARGS+=("-cpu" "max")
fi

# Boot Media Configuration
if [ "$BOOT_SRC" == "iso" ]; then
    if [ ! -f "$ISO_PATH" ]; then
        echo -e "${RED}Error: ISO file not found at: ${ISO_PATH}${NC}" >&2
        echo "Run './build.sh' to compile the ISO first."
        exit 1
    fi
    echo -e "  ${CYAN}•${NC} Booting from Live ISO: ${ISO_PATH}"
    QEMU_ARGS+=("-cdrom" "${ISO_PATH}" "-boot" "d")
else
    if [ ! -f "$DISK_PATH" ]; then
        echo -e "${RED}Error: Target disk not found at: ${DISK_PATH}${NC}" >&2
        echo "Creating a 20GB disk image now..."
        qemu-img create -f qcow2 "${DISK_PATH}" 20G
    fi
    echo -e "  ${CYAN}•${NC} Booting from Installed Disk: ${DISK_PATH}"
    QEMU_ARGS+=("-drive" "file=${DISK_PATH},format=qcow2,if=virtio" "-boot" "c")
fi

# Display Mode Configuration
case "$DISPLAY_MODE" in
    gui)
        echo -e "  ${CYAN}•${NC} Display: Native Graphical Window (virtio-vga)"
        QEMU_ARGS+=("-vga" "virtio" "-display" "default")
        ;;
    none)
        echo -e "  ${CYAN}•${NC} Display: Headless / Background (Serial on stdio)"
        QEMU_ARGS+=("-display" "none" "-serial" "stdio")
        ;;
    curses)
        echo -e "  ${CYAN}•${NC} Display: Terminal Console (curses)"
        QEMU_ARGS+=("-display" "curses")
        ;;
esac

echo ""
echo -e "${GREEN}${BOLD}Server Port Mappings Active:${NC}"
echo -e "  ${BOLD}SSH Access:${NC}    ssh -p 2222 ri@localhost"
echo -e "  ${BOLD}Web Server:${NC}    http://localhost:8080"
echo -e "  ${BOLD}Web Secure:${NC}    https://localhost:8443"
echo -e "  ${BOLD}Cockpit GUI:${NC}   http://localhost:9090"
echo "------------------------------------------------------------"
echo "Press Ctrl+A then X to exit QEMU if running in terminal mode."
echo "============================================================"
echo ""

exec qemu-system-x86_64 "${QEMU_ARGS[@]}"
