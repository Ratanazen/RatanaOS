#!/usr/bin/env bash
# ==============================================================================
# RiOS Custom ISO Builder (1.0.0)
# Enables building customized RiOS ISOs by enabling/disabling modular profiles:
#   - Server:   Nginx, OpenSSH, Cockpit, Docker, UFW, Fail2ban, WireGuard
#   - Cyber:    Nmap, Wireshark, John, Hydra, SQLMap, Binwalk, Radare2
#   - Dev:      Python, Node, Go, Rust, Neovim, Tmux, Git, Podman
#   - Desktops: GNOME, KDE Plasma, Hyprland, Sway
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
PKG_LIST_DIR="${ROOT_DIR}/rios-live/config/package-lists"

BOLD='\033[1m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'

print_banner() {
    echo -e "${BLUE}${BOLD}"
    echo "╔══════════════════════════════════════════════════════════╗"
    echo "║              RiOS Custom ISO Builder (1.0.0)             ║"
    echo "║       Build Tailored Server, Cyber, Dev & Desktop ISOs   ║"
    echo "╚══════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

usage() {
    print_banner
    echo -e "${BOLD}Usage:${NC} $0 [PRESET | --custom [OPTIONS]]"
    echo ""
    echo -e "${BOLD}Presets:${NC}"
    echo "  all                  Full RiOS Edition with all suites (Server + Cyber + Dev + All Desktops)"
    echo "  server               Dedicated Server Edition (Nginx, SSH, Cockpit, Docker, Wireguard, UFW)"
    echo "  cyber                Cybersecurity & Penetration Testing Edition"
    echo "  dev                  Software Engineering & Developer Workstation"
    echo "  minimal              Minimal Base with Hyprland & Sway only"
    echo ""
    echo -e "${BOLD}Custom Modular Flags:${NC}"
    echo "  --enable-server      Include Server Suite (SSH, Nginx, Docker, Cockpit, Firewalls)"
    echo "  --disable-server     Exclude Server Suite"
    echo "  --enable-cyber       Include Cybersecurity & Hacking Tools"
    echo "  --disable-cyber      Exclude Cybersecurity Suite"
    echo "  --enable-dev         Include Development Runtimes & Compilers"
    echo "  --disable-dev        Exclude Development Suite"
    echo "  --enable-gnome       Include GNOME Desktop"
    echo "  --disable-gnome      Exclude GNOME Desktop"
    echo "  --enable-kde         Include KDE Plasma Desktop"
    echo "  --disable-kde        Exclude KDE Plasma Desktop"
    echo "  --enable-wayland     Include Hyprland & Sway Compositors"
    echo "  --disable-wayland    Exclude Wayland Compositors"
    echo "  --status             Show currently active package list profiles"
    echo "  --clean              Clean build cache and artifacts before building"
    echo "  --no-build           Apply profile selections without starting live-build"
    echo "  -h, --help           Show this help"
    echo ""
    exit 0
}

set_profile() {
    local name="$1"
    local enable="$2"
    local file_active="${PKG_LIST_DIR}/rios-${name}.list.chroot"
    local file_disabled="${PKG_LIST_DIR}/rios-${name}.list.chroot.disabled"

    if [ "$enable" -eq 1 ]; then
        if [ -f "$file_disabled" ]; then
            mv "$file_disabled" "$file_active"
        fi
        echo -e "  ${GREEN}✓${NC} Profile ${BOLD}${name}${NC}: ${GREEN}ENABLED${NC}"
    else
        if [ -f "$file_active" ]; then
            mv "$file_active" "$file_disabled"
        fi
        echo -e "  ${YELLOW}○${NC} Profile ${BOLD}${name}${NC}: ${YELLOW}DISABLED${NC}"
    fi
}

show_status() {
    print_banner
    echo -e "${BOLD}Current Package List Profiles in ${PKG_LIST_DIR}:${NC}"
    for profile in server cyber dev gnome kde wayland desktop; do
        if [ -f "${PKG_LIST_DIR}/rios-${profile}.list.chroot" ]; then
            echo -e "  [${GREEN}ON${NC}]  rios-${profile}"
        else
            echo -e "  [${RED}OFF${NC}] rios-${profile}"
        fi
    done
    echo ""
}

# Defaults
DO_BUILD=1
DO_CLEAN=0

if [ $# -eq 0 ]; then
    usage
fi

case "$1" in
    all|full)
        print_banner
        echo -e "${BOLD}Configuring PRESET: Full / All Editions...${NC}"
        set_profile "server" 1
        set_profile "cyber" 1
        set_profile "dev" 1
        set_profile "gnome" 1
        set_profile "kde" 1
        set_profile "wayland" 1
        set_profile "desktop" 1
        ;;
    server)
        print_banner
        echo -e "${BOLD}Configuring PRESET: Dedicated Server Edition...${NC}"
        set_profile "server" 1
        set_profile "dev" 1
        set_profile "cyber" 0
        set_profile "gnome" 0
        set_profile "kde" 0
        set_profile "wayland" 0
        set_profile "desktop" 0
        ;;
    cyber)
        print_banner
        echo -e "${BOLD}Configuring PRESET: Cybersecurity & Pen-Testing Edition...${NC}"
        set_profile "cyber" 1
        set_profile "dev" 1
        set_profile "server" 1
        set_profile "wayland" 1
        set_profile "desktop" 1
        set_profile "gnome" 0
        set_profile "kde" 0
        ;;
    dev)
        print_banner
        echo -e "${BOLD}Configuring PRESET: Developer Workstation...${NC}"
        set_profile "dev" 1
        set_profile "server" 1
        set_profile "wayland" 1
        set_profile "desktop" 1
        set_profile "cyber" 0
        set_profile "gnome" 0
        set_profile "kde" 0
        ;;
    minimal)
        print_banner
        echo -e "${BOLD}Configuring PRESET: Minimal Base + Wayland...${NC}"
        set_profile "wayland" 1
        set_profile "desktop" 1
        set_profile "server" 0
        set_profile "cyber" 0
        set_profile "dev" 0
        set_profile "gnome" 0
        set_profile "kde" 0
        ;;
    status)
        show_status
        exit 0
        ;;
    --status)
        show_status
        exit 0
        ;;
    clean)
        "${ROOT_DIR}/build.sh" clean
        exit 0
        ;;
    custom|--custom|*)
        print_banner
        echo -e "${BOLD}Applying Custom Profile Adjustments...${NC}"
        while [[ $# -gt 0 ]]; do
            case "$1" in
                --enable-server)   set_profile "server" 1; shift ;;
                --disable-server)  set_profile "server" 0; shift ;;
                --enable-cyber)    set_profile "cyber" 1; shift ;;
                --disable-cyber)   set_profile "cyber" 0; shift ;;
                --enable-dev)      set_profile "dev" 1; shift ;;
                --disable-dev)     set_profile "dev" 0; shift ;;
                --enable-gnome)    set_profile "gnome" 1; shift ;;
                --disable-gnome)   set_profile "gnome" 0; shift ;;
                --enable-kde)      set_profile "kde" 1; shift ;;
                --disable-kde)     set_profile "kde" 0; shift ;;
                --enable-wayland)  set_profile "wayland" 1; shift ;;
                --disable-wayland) set_profile "wayland" 0; shift ;;
                --clean)           DO_CLEAN=1; shift ;;
                --no-build)        DO_BUILD=0; shift ;;
                -h|--help)         usage ;;
                custom|--custom)   shift ;;
                *) echo "Unknown option: $1"; usage ;;
            esac
        done
        ;;
esac

if [ "$DO_CLEAN" -eq 1 ]; then
    echo -e "\n${BOLD}Cleaning build environment...${NC}"
    "${ROOT_DIR}/build.sh" clean
fi

if [ "$DO_BUILD" -eq 1 ]; then
    echo -e "\n${BOLD}==> Initiating Live-Build Engine...${NC}"
    "${ROOT_DIR}/tools/fast-live-build.sh"
else
    echo -e "\n${GREEN}Profiles configured. Build skipped (--no-build).${NC}"
fi
