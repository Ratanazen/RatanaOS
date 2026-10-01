#!/usr/bin/env bash
# ==============================================================================
# RiOS Master Build & Configuration Pipeline (1.0.0)
# Supports: Full / All Edition, Server Edition, Cyber Edition, Dev, & Custom Modular Builds
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
        echo "Starting RiOS Dedicated Server Build..."
        "${SCRIPT_DIR}/scripts/build_custom.sh" server
        ;;
    cyber)
        echo "Starting RiOS Cybersecurity & Pen-Testing Build..."
        "${SCRIPT_DIR}/scripts/build_custom.sh" cyber
        ;;
    dev)
        echo "Starting RiOS Developer Workstation Build..."
        "${SCRIPT_DIR}/scripts/build_custom.sh" dev
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
        echo "Commands & Presets:"
        echo "  all                  Build Full Edition (All Suites enabled - default)"
        echo "  server               Build Dedicated Server Edition"
        echo "  cyber                Build Cybersecurity & Pen-Testing Edition"
        echo "  dev                  Build Developer Workstation Edition"
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
