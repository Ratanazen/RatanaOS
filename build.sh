#!/usr/bin/env bash
# ==============================================================================
# RiOS Master Build & Configuration Pipeline (2026 Full System)
# Supports: Full / All Edition, Server Edition, Cyber Edition, Dev, & 4-in-1 Batch Validation
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

case "${1:-all}" in
    clean)
        echo "Cleaning build directory & live-build temporary states..."
        rm -rf "${SCRIPT_DIR}/build/"*.iso "${SCRIPT_DIR}/build/"*.sha256 \
               "${SCRIPT_DIR}/rios-live/.build" "${SCRIPT_DIR}/rios-live/binary" \
               "${SCRIPT_DIR}/rios-live/chroot" "${SCRIPT_DIR}/rios-live/cache" 2>/dev/null || true
        echo "Clean completed."
        exit 0
        ;;
    all|full)
        exec "${SCRIPT_DIR}/scripts/build_full_system.sh"
        ;;
    server)
        echo "Starting RiOS 2026 Dedicated Server Build..."
        "${SCRIPT_DIR}/scripts/build_custom.sh" server
        ;;
    cyber)
        echo "Starting RiOS 2026 Cybersecurity & Pen-Testing Build..."
        "${SCRIPT_DIR}/scripts/build_custom.sh" cyber
        ;;
    dev)
        echo "Starting RiOS 2026 Developer Workstation Build..."
        "${SCRIPT_DIR}/scripts/build_custom.sh" dev
        ;;
    all4|--all4|4|editions)
        echo "============================================================"
        echo " RiOS 2026 Full System — 4-Edition Architecture Orchestrator"
        echo " Validating all 4 Editions: Server, Cyber, Dev, and Full"
        echo "============================================================"
        for edition in server cyber dev all; do
            echo -e "\n--> Configuring & Verifying Edition: ${edition}..."
            "${SCRIPT_DIR}/scripts/build_custom.sh" "${edition}" --no-build
        done
        echo -e "\n============================================================"
        echo " ✔ All 4 Editions Successfully Configured & Validated!"
        echo " System reset to Full Edition (all 4 suites active)."
        echo "============================================================"
        exit 0
        ;;
    cloud)
        echo "Starting RiOS 2026 Cloud QCOW2 Image Build..."
        exec "${SCRIPT_DIR}/scripts/build_cloud_image.sh"
        ;;
    netboot|pxe)
        echo "Starting RiOS 2026 Netboot & PXE Asset Build..."
        exec "${SCRIPT_DIR}/scripts/build_netboot.sh"
        ;;
    container|docker|podman)
        echo "Starting RiOS 2026 Container Image Build..."
        exec "${SCRIPT_DIR}/scripts/build_container_image.sh"
        ;;
    custom|--custom)
        shift 1 || true
        "${SCRIPT_DIR}/scripts/build_custom.sh" custom "$@"
        ;;
    status|--status)
        "${SCRIPT_DIR}/scripts/build_custom.sh" --status
        ;;
    -h|--help|help)
        echo "Usage: ./build.sh [COMMAND | PRESET]"
        echo ""
        echo "RiOS 2026 Core Editions (All 4):"
        echo "  all                  Build Full 2026 Edition (All 4 Suites enabled - default)"
        echo "  server               Build Dedicated Server 2026 Edition (Nginx, SSH, Cockpit, Docker)"
        echo "  cyber                Build Cybersecurity & Pen-Testing 2026 Edition (76+ tools, Kali parity)"
        echo "  dev                  Build Developer Workstation 2026 Edition (Compilers, runtimes, containers)"
        echo "  all4                 Verify & validate all 4 editions sequentially"
        echo ""
        echo "Appliance & Infrastructure Targets:"
        echo "  cloud                Build Cloud-Init QCOW2 Virtual Appliance"
        echo "  netboot              Build iPXE & PXELINUX Network Boot Assets"
        echo "  container            Build Docker & Podman Container Image"
        echo "  custom [options]     Build with custom modular profiles"
        echo "  status               Show active package profiles"
        echo "  clean                Clean temporary cache and build artifacts"
        echo "  help                 Show this help message"
        exit 0
        ;;
    *)
        echo "Unknown option: $1"
        echo "Run './build.sh help' for options."
        exit 1
        ;;
esac
