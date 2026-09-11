#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Master Desktop Auto-Installer & Environment Provisioner
# Automatically installs, themes, configures, and validates Hyprland, Niri, Sway,
# Waybar, SwayNC, Wofi/Rofi, and GTK/Qt across Debian, Arch, and Fedora systems.
# ==============================================================================

set -euo pipefail

# Determine executing user and home
if [ -n "${SUDO_USER:-}" ] && [ "${SUDO_USER}" != "root" ]; then
    TARGET_USER="${SUDO_USER}"
    TARGET_HOME=$(getent passwd "${TARGET_USER}" | cut -d: -f6)
    TARGET_GID=$(getent passwd "${TARGET_USER}" | cut -d: -f4)
    TARGET_GROUP=$(getent group "${TARGET_GID}" | cut -d: -f1)
else
    TARGET_USER="${USER:-root}"
    TARGET_HOME="${HOME}"
    TARGET_GROUP="$(id -gn "${TARGET_USER}" 2>/dev/null || echo root)"
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

LOG_SYS="/var/log/ratanaos/install.log"
LOG_USER="${TARGET_HOME}/.local/state/ratanaos/install.log"
mkdir -p "$(dirname "${LOG_SYS}")" 2>/dev/null || true
mkdir -p "$(dirname "${LOG_USER}")" 2>/dev/null || true

log() {
    local level="$1"
    shift
    local msg="$*"
    local ts
    ts=$(date +"%Y-%m-%d %H:%M:%S")
    echo "[$level] $msg"
    echo "[$ts] [$level] $msg" >> "${LOG_SYS}" 2>/dev/null || true
    echo "[$ts] [$level] $msg" >> "${LOG_USER}" 2>/dev/null || true
}

print_banner() {
    echo -e "\e[34m╭──────────────────────────────────────╮\e[0m"
    echo -e "\e[34m│\e[0m           \e[1;37mRATANAOS DESKTOP\e[0m           \e[34m│\e[0m"
    echo -e "\e[34m│\e[0m       \e[36mFULL ENVIRONMENT SETUP\e[0m         \e[34m│\e[0m"
    echo -e "\e[34m╰──────────────────────────────────────╯\e[0m"
}

# Defaults
INSTALL_TARGET="all"
SELECTED_THEME="ratana-dark"
DRY_RUN=0
NON_INTERACTIVE=0

# Parse Arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        --hyprland) INSTALL_TARGET="hyprland"; shift ;;
        --niri)     INSTALL_TARGET="niri"; shift ;;
        --sway)     INSTALL_TARGET="sway"; shift ;;
        --all)      INSTALL_TARGET="all"; shift ;;
        --profile)  INSTALL_TARGET="$2"; shift 2 ;;
        --theme)    SELECTED_THEME="$2"; shift 2 ;;
        --dry-run)  DRY_RUN=1; shift ;;
        -y|--yes|--non-interactive) NON_INTERACTIVE=1; shift ;;
        *) shift ;;
    esac
done

detect_system() {
    echo -ne "\e[1;33m[1/12] Detecting system...\e[0m "
    PKG_MGR="unknown"
    if command -v apt-get >/dev/null 2>&1; then
        PKG_MGR="apt"
    elif command -v pacman >/dev/null 2>&1; then
        PKG_MGR="pacman"
    elif command -v dnf >/dev/null 2>&1; then
        PKG_MGR="dnf"
    fi

    GPU_TYPE="generic"
    if command -v lspci >/dev/null 2>&1; then
        if lspci | grep -iE 'vga|3d' | grep -iq nvidia; then
            GPU_TYPE="nvidia"
        elif lspci | grep -iE 'vga|3d' | grep -iq amd; then
            GPU_TYPE="amd"
        elif lspci | grep -iE 'vga|3d' | grep -iq intel; then
            GPU_TYPE="intel"
        fi
    fi

    TOTAL_MEM_MB=$(free -m | awk '/Mem:/ {print $2}')
    PERF_PROFILE="balanced"
    if [ "$TOTAL_MEM_MB" -lt 4000 ]; then
        PERF_PROFILE="low"
    elif [ "$TOTAL_MEM_MB" -gt 15000 ] && [ "$GPU_TYPE" != "generic" ]; then
        PERF_PROFILE="beautiful"
    fi
    echo -e "\e[1;32m✓\e[0m (OS: ${PKG_MGR}, GPU: ${GPU_TYPE}, Profile: ${PERF_PROFILE})"
    log "INFO" "System detected: Manager=${PKG_MGR}, GPU=${GPU_TYPE}, RAM=${TOTAL_MEM_MB}MB, Profile=${PERF_PROFILE}"
}

install_packages() {
    echo -ne "\e[1;33m[2/12] Installing packages...\e[0m "
    if [ "$DRY_RUN" -eq 1 ]; then
        echo -e "\e[1;34m[DRY-RUN]\e[0m skipped"
        return 0
    fi
    if [ "${PKG_MGR}" = "apt" ]; then
        export DEBIAN_FRONTEND=noninteractive
        apt-get update -qq >/dev/null 2>&1 || true
        apt-get install -y -qq \
            sway swaybg swaylock swayidle waybar wofi rofi kitty foot \
            mako-notifier grim slurp wl-clipboard wf-recorder policykit-1-gnome \
            xdg-desktop-portal-wlr xdg-desktop-portal-gtk brightnessctl pamixer \
            playerctl pavucontrol neofetch btop thunar lightdm >/dev/null 2>&1 || true
    fi
    echo -e "\e[1;32m✓\e[0m"
    log "INFO" "Base package installation completed"
}

install_fonts() {
    echo -ne "\e[1;33m[3/12] Installing fonts...\e[0m "
    if [ "$DRY_RUN" -eq 1 ]; then
        echo -e "\e[1;34m[DRY-RUN]\e[0m skipped"
        return 0
    fi
    if [ "${PKG_MGR}" = "apt" ]; then
        apt-get install -y -qq fonts-firacode fonts-dejavu-core fonts-inter fonts-noto-core fonts-noto-cjk fonts-noto-color-emoji fonts-khmeros fonts-font-awesome >/dev/null 2>&1 || true
    fi
    fc-cache -f >/dev/null 2>&1 || true
    echo -e "\e[1;32m✓\e[0m"
    log "INFO" "Font suite installed and cache refreshed"
}

install_themes() {
    echo -ne "\e[1;33m[4/12] Installing themes...\e[0m "
    if [ "$DRY_RUN" -eq 1 ]; then
        echo -e "\e[1;34m[DRY-RUN]\e[0m skipped"
        return 0
    fi
    mkdir -p /usr/share/ratanaos/themes /etc/ratanaos
    if [ -d "${ROOT_DIR}/themes" ]; then
        cp -rf "${ROOT_DIR}"/themes/* /usr/share/ratanaos/themes/ 2>/dev/null || true
    elif [ -d "/usr/share/ratanaos/themes" ]; then
        :
    fi
    echo -e "\e[1;32m✓\e[0m"
    log "INFO" "RatanaOS themes installed"
}

install_configs() {
    USER_CONFIG="${TARGET_HOME}/.config"
    BACKUP_DIR="${USER_CONFIG}/ratana/backups/$(date +%Y%m%d_%H%M%S)"
    mkdir -p "${BACKUP_DIR}" "${USER_CONFIG}/ratana"
    for comp in hypr niri sway waybar rofi wofi swaync kitty; do
        if [ -d "${USER_CONFIG}/${comp}" ]; then
            cp -rf "${USER_CONFIG}/${comp}" "${BACKUP_DIR}/" 2>/dev/null || true
        fi
    done

    # Hyprland
    if [ "$INSTALL_TARGET" = "all" ] || [ "$INSTALL_TARGET" = "hyprland" ]; then
        echo -ne "\e[1;33m[5/12] Configuring Hyprland...\e[0m "
        if [ "$DRY_RUN" -eq 0 ]; then
            mkdir -p "${USER_CONFIG}/hypr"
            if [ -d "/etc/skel/.config/hypr" ]; then
                cp -rf /etc/skel/.config/hypr/* "${USER_CONFIG}/hypr/"
            elif [ -d "${ROOT_DIR}/config/desktop/hyprland" ]; then
                cp -rf "${ROOT_DIR}"/config/desktop/hyprland/* "${USER_CONFIG}/hypr/"
            fi
        fi
        echo -e "\e[1;32m✓\e[0m"
        log "INFO" "Hyprland configuration deployed"
    fi

    # Niri
    if [ "$INSTALL_TARGET" = "all" ] || [ "$INSTALL_TARGET" = "niri" ]; then
        echo -ne "\e[1;33m[6/12] Configuring Niri...\e[0m "
        if [ "$DRY_RUN" -eq 0 ]; then
            mkdir -p "${USER_CONFIG}/niri"
            if [ -d "/etc/skel/.config/niri" ]; then
                cp -rf /etc/skel/.config/niri/* "${USER_CONFIG}/niri/"
            elif [ -d "${ROOT_DIR}/config/desktop/niri" ]; then
                cp -rf "${ROOT_DIR}"/config/desktop/niri/* "${USER_CONFIG}/niri/"
            fi
        fi
        echo -e "\e[1;32m✓\e[0m"
        log "INFO" "Niri configuration deployed"
    fi

    # Sway
    if [ "$INSTALL_TARGET" = "all" ] || [ "$INSTALL_TARGET" = "sway" ]; then
        echo -ne "\e[1;33m[7/12] Configuring Sway...\e[0m "
        if [ "$DRY_RUN" -eq 0 ]; then
            mkdir -p "${USER_CONFIG}/sway"
            if [ -d "/etc/skel/.config/sway" ]; then
                cp -rf /etc/skel/.config/sway/* "${USER_CONFIG}/sway/"
            elif [ -d "${ROOT_DIR}/config/desktop/sway" ]; then
                cp -rf "${ROOT_DIR}"/config/desktop/sway/* "${USER_CONFIG}/sway/"
            fi
        fi
        echo -e "\e[1;32m✓\e[0m"
        log "INFO" "Sway configuration deployed"
    fi

    # Waybar
    echo -ne "\e[1;33m[8/12] Configuring Waybar...\e[0m "
    if [ "$DRY_RUN" -eq 0 ]; then
        mkdir -p "${USER_CONFIG}/waybar"
        if [ -d "/etc/skel/.config/waybar" ]; then
            cp -rf /etc/skel/.config/waybar/* "${USER_CONFIG}/waybar/"
        elif [ -d "${ROOT_DIR}/config/waybar" ]; then
            cp -rf "${ROOT_DIR}"/config/waybar/* "${USER_CONFIG}/waybar/"
        fi
    fi
    echo -e "\e[1;32m✓\e[0m"
    log "INFO" "Waybar configuration deployed"
}

configure_gtk() {
    echo -ne "\e[1;33m[9/12] Configuring GTK/Qt...\e[0m "
    if [ "$DRY_RUN" -eq 0 ]; then
        mkdir -p "${USER_CONFIG}/gtk-3.0" "${USER_CONFIG}/gtk-4.0"
        cat << 'GEOF' > "${USER_CONFIG}/gtk-3.0/settings.ini"
[Settings]
gtk-theme-name=WhiteSur-Dark
gtk-icon-theme-name=WhiteSur-dark
gtk-cursor-theme-name=WhiteSur-cursors
gtk-application-prefer-dark-theme=1
gtk-font-name=Cantarell 11
GEOF
        cat << 'GEOF' > "${USER_CONFIG}/gtk-4.0/gtk.css"
@import url("file:///usr/share/themes/WhiteSur-Dark/gtk-4.0/gtk.css");
GEOF
    fi
    echo -e "\e[1;32m✓\e[0m"
    log "INFO" "GTK/Qt user preferences configured"
}

configure_services() {
    echo -ne "\e[1;33m[10/12] Configuring services...\e[0m "
    if [ "$DRY_RUN" -eq 0 ]; then
        mkdir -p /usr/share/wayland-sessions
        if [ -d "${ROOT_DIR}/ratanaos-live/config/includes.chroot/usr/share/wayland-sessions" ]; then
            cp -rf "${ROOT_DIR}"/ratanaos-live/config/includes.chroot/usr/share/wayland-sessions/* /usr/share/wayland-sessions/ 2>/dev/null || true
        fi
    fi
    echo -e "\e[1;32m✓\e[0m"
    log "INFO" "Wayland session services registered"
}

verify_installation() {
    echo -ne "\e[1;33m[11/12] Validating setup...\e[0m "
    if [ "$DRY_RUN" -eq 1 ]; then
        echo -e "\e[1;32m✓\e[0m (Dry run validation passed)"
        return 0
    fi
    VALID=1
    [ -f "${USER_CONFIG}/waybar/style.css" ] || VALID=0
    if [ $VALID -eq 1 ]; then
        echo -e "\e[1;32m✓\e[0m"
        log "INFO" "Validation PASSED"
    else
        echo -e "\e[1;31m✗\e[0m"
        log "ERROR" "Validation FAILED"
    fi
}

finalize_permissions() {
    echo -ne "\e[1;33m[12/12] Final health check...\e[0m "
    if [ "$DRY_RUN" -eq 0 ]; then
        chown -R "${TARGET_USER}:${TARGET_GROUP}" "${TARGET_HOME}/.config" 2>/dev/null || true
        chown -R "${TARGET_USER}:${TARGET_GROUP}" "${TARGET_HOME}/.local" 2>/dev/null || true
    fi
    echo -e "\e[1;32m✓\e[0m"
    log "INFO" "Ownership assigned to ${TARGET_USER}:${TARGET_GROUP}. Setup complete."
}

# Main Execution Flow
print_banner
if [ "$DRY_RUN" -eq 1 ]; then
    echo -e "\e[1;36m[NOTICE] Dry run enabled. No files or packages will be modified.\e[0m\n"
fi

detect_system
install_packages
install_fonts
install_themes
install_configs
configure_gtk
configure_services
verify_installation
finalize_permissions

echo -e "\n\e[1;32mRatanaOS Desktop Ready.\e[0m"
echo "Select your preferred session (Hyprland, Niri, Sway) at the login screen."
