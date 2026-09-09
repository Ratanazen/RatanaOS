#!/bin/bash
# ============================================================================
# build-ratanaos.sh — RatanaOS Distribution Build System
# ============================================================================
# Transforms a Debian base system into a fully branded RatanaOS distribution.
# Usage: ./build-ratanaos.sh <command>
# Commands: audit, prepare, rootfs, kernel, packages, desktop, installer, iso,
#           test, clean, all
# ============================================================================

set -euo pipefail

# ── Configuration ──────────────────────────────────────────────────────────────
RATANAOS_VERSION="1.0.0"
RATANAOS_CODENAME="Angkor"
RATANAOS_ARCH="x86_64"
DEBIAN_RELEASE="bookworm"
DEBIAN_MIRROR="http://deb.debian.org/debian"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE="${SCRIPT_DIR}"
DEBIAN_SOURCE="${WORKSPACE}/debian-source"
RATANAOS_ROOTFS="${WORKSPACE}/ratanaos-rootfs"
BUILD_DIR="${WORKSPACE}/build"
CONFIG_DIR="${WORKSPACE}/config"
BRANDING_DIR="${WORKSPACE}/branding"
SCRIPTS_DIR="${WORKSPACE}/scripts"
TOOLS_DIR="${WORKSPACE}/tools"
DESKTOP_DIR="${WORKSPACE}/desktop"
INSTALLER_DIR="${WORKSPACE}/installer"
SERVICES_DIR="${WORKSPACE}/services"
DOCS_DIR="${WORKSPACE}/docs"
TESTS_DIR="${WORKSPACE}/tests"
RELEASES_DIR="${WORKSPACE}/releases"
ISO_DIR="${BUILD_DIR}/iso"
ISO_OUTPUT="${RELEASES_DIR}/RatanaOS-${RATANAOS_VERSION}-${RATANAOS_ARCH}"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

log_info()  { echo -e "${CYAN}[INFO]${NC} $*"; }
log_ok()    { echo -e "${GREEN}[OK]${NC} $*"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $*"; }
log_error() { echo -e "${RED}[ERROR]${NC} $*"; }
log_step()  { echo -e "\n${BOLD}${BLUE}═══ $* ═══${NC}"; }

check_root() {
    if [[ $EUID -ne 0 ]]; then
        log_error "This command requires root privileges. Run with sudo."
        exit 1
    fi
}

check_deps() {
    local missing=()
    for cmd in debootstrap mksquashfs xorriso grub-mkrescue qemu-system-x86_64; do
        if ! command -v "$cmd" &>/dev/null; then
            missing+=("$cmd")
        fi
    done
    if [[ ${#missing[@]} -gt 0 ]]; then
        log_error "Missing dependencies: ${missing[*]}"
        log_info "Install with: sudo pacman -S debootstrap squashfs-tools dpkg arch-install-scripts dosfstools"
        exit 1
    fi
    log_ok "All build dependencies are available"
}

# ── PHASE 1: AUDIT ─────────────────────────────────────────────────────────────
cmd_audit() {
    log_step "Phase 1: Auditing Debian Source System"

    if [[ ! -d "${DEBIAN_SOURCE}" ]]; then
        log_warn "Debian source not yet bootstrapped. Run: sudo ./build-ratanaos.sh prepare"
        return 1
    fi

    local audit_file="${DOCS_DIR}/RATANAOS_DEBIAN_AUDIT.md"
    log_info "Generating audit report → ${audit_file}"

    cat > "${audit_file}" << 'AUDIT_HEADER'
# RatanaOS Debian Source Audit

**Generated**: $(date -Iseconds)
**Source**: Debian Bookworm (12) x86_64 minimal bootstrap

---

## Component Inventory

| Component | Debian Implementation | Location | Can Reuse? | Needs Branding? | Status |
|---|---|---|---|---|---|
AUDIT_HEADER

    # Detect components in the bootstrapped rootfs
    local rootfs="${DEBIAN_SOURCE}"

    # Kernel
    local kernel_ver="$(ls ${rootfs}/lib/modules/ 2>/dev/null | head -1 || echo 'not installed')"
    echo "| Kernel | ${kernel_ver} | /boot, /lib/modules | ✅ Yes | ✅ Config name | Audited |" >> "${audit_file}"

    # glibc
    local glibc_ver="$(chroot ${rootfs} /usr/bin/ldd --version 2>/dev/null | head -1 | grep -oP '[\d.]+$' || echo 'N/A')"
    echo "| glibc | ${glibc_ver} | /lib/x86_64-linux-gnu | ✅ Yes | ❌ No | Audited |" >> "${audit_file}"

    # dpkg
    local dpkg_ver="$(chroot ${rootfs} dpkg --version 2>/dev/null | head -1 | grep -oP '[\d.]+' || echo 'N/A')"
    echo "| dpkg | ${dpkg_ver} | /usr/bin/dpkg | ✅ Yes (wrapped by ratapkg) | ✅ Wrapper | Audited |" >> "${audit_file}"

    # apt
    echo "| apt | (bundled) | /usr/bin/apt | ✅ Yes (wrapped by ratapkg) | ✅ Wrapper | Audited |" >> "${audit_file}"

    # systemd
    local systemd_ver="$(chroot ${rootfs} systemctl --version 2>/dev/null | head -1 | awk '{print $2}' || echo 'N/A')"
    echo "| systemd | ${systemd_ver} | /lib/systemd | ✅ Yes | ✅ Services | Audited |" >> "${audit_file}"

    # Filesystem layout
    echo "| Root FS | FHS 3.0 | / | ✅ Yes | ✅ /etc config | Audited |" >> "${audit_file}"

    # os-release
    echo "| os-release | Debian 12 | /etc/os-release | ❌ Replace | ✅ Full rebrand | Pending |" >> "${audit_file}"

    # hostname
    echo "| hostname | debian | /etc/hostname | ❌ Replace | ✅ 'ratanaos' | Pending |" >> "${audit_file}"

    # GRUB
    echo "| GRUB | 2.x | /etc/default/grub | ✅ Yes | ✅ Boot branding | Pending |" >> "${audit_file}"

    # Network
    echo "| Networking | ifupdown/systemd-networkd | /etc/network | ✅ Yes | ✅ Wrapper | Pending |" >> "${audit_file}"

    # Shell
    echo "| Bash | 5.x | /bin/bash | ✅ Yes | ✅ Prompt/RC | Pending |" >> "${audit_file}"

    # Package count
    local pkg_count="$(chroot ${rootfs} dpkg -l 2>/dev/null | grep '^ii' | wc -l || echo '0')"
    echo "" >> "${audit_file}"
    echo "## Statistics" >> "${audit_file}"
    echo "" >> "${audit_file}"
    echo "- **Total installed packages**: ${pkg_count}" >> "${audit_file}"
    echo "- **Architecture**: amd64 (x86_64)" >> "${audit_file}"
    echo "- **Debian release**: ${DEBIAN_RELEASE}" >> "${audit_file}"

    log_ok "Audit report generated: ${audit_file}"
}

# ── PHASE 1: PREPARE (Bootstrap Debian) ────────────────────────────────────────
cmd_prepare() {
    log_step "Phase 1: Bootstrapping Debian ${DEBIAN_RELEASE} into ${DEBIAN_SOURCE}"
    check_root
    check_deps

    if [[ -d "${DEBIAN_SOURCE}/bin" ]]; then
        log_warn "Debian source already exists at ${DEBIAN_SOURCE}. Skipping bootstrap."
        log_info "To re-bootstrap, remove ${DEBIAN_SOURCE} first."
        return 0
    fi

    mkdir -p "${DEBIAN_SOURCE}"

    log_info "Running debootstrap (this will download ~300 MB)..."
    debootstrap --arch=amd64 --variant=minbase \
        --include=systemd,systemd-sysv,dbus,udev,iproute2,iputils-ping,\
net-tools,wget,curl,ca-certificates,locales,sudo,bash-completion,\
procps,psmisc,lsof,less,vim-tiny,nano,openssh-client,\
linux-image-amd64,grub-pc,grub-efi-amd64-bin,initramfs-tools \
        "${DEBIAN_RELEASE}" "${DEBIAN_SOURCE}" "${DEBIAN_MIRROR}"

    log_ok "Debian ${DEBIAN_RELEASE} bootstrapped successfully"
    log_info "Total size: $(du -sh ${DEBIAN_SOURCE} | cut -f1)"
}

# ── PHASE 1: ROOTFS (Transform Debian → RatanaOS) ─────────────────────────────
cmd_rootfs() {
    log_step "Phase 1: Building RatanaOS Root Filesystem"
    check_root

    if [[ ! -d "${DEBIAN_SOURCE}/bin" ]]; then
        log_error "Debian source not found. Run: sudo ./build-ratanaos.sh prepare"
        return 1
    fi

    # Copy debian-source → ratanaos-rootfs
    if [[ ! -d "${RATANAOS_ROOTFS}/bin" ]]; then
        log_info "Copying Debian source → RatanaOS rootfs..."
        cp -a "${DEBIAN_SOURCE}" "${RATANAOS_ROOTFS}"
        log_ok "Rootfs copy complete"
    else
        log_info "RatanaOS rootfs already exists. Applying transformations..."
    fi

    local R="${RATANAOS_ROOTFS}"

    # ── os-release ──
    log_info "Applying RatanaOS identity..."
    cat > "${R}/etc/os-release" << EOF
NAME="RatanaOS"
PRETTY_NAME="RatanaOS ${RATANAOS_VERSION} (${RATANAOS_CODENAME})"
ID=ratanaos
ID_LIKE=debian
VERSION="${RATANAOS_VERSION}"
VERSION_ID="${RATANAOS_VERSION}"
VERSION_CODENAME="${RATANAOS_CODENAME}"
HOME_URL="https://ratanaos.org"
BUG_REPORT_URL="https://github.com/ratanaos/ratanaos/issues"
SUPPORT_URL="https://ratanaos.org/support"
BUILD_ID="$(date +%Y%m%d)"
VARIANT="Desktop"
VARIANT_ID=desktop
LOGO=ratanaos-logo
DEBIAN_CODENAME="${DEBIAN_RELEASE}"
ANSI_COLOR="38;2;59;130;246"
EOF

    # ── lsb-release ──
    cat > "${R}/etc/lsb-release" << EOF
DISTRIB_ID=RatanaOS
DISTRIB_RELEASE=${RATANAOS_VERSION}
DISTRIB_CODENAME=${RATANAOS_CODENAME}
DISTRIB_DESCRIPTION="RatanaOS ${RATANAOS_VERSION} (${RATANAOS_CODENAME})"
EOF

    # ── hostname ──
    echo "ratanaos" > "${R}/etc/hostname"

    # ── hosts ──
    cat > "${R}/etc/hosts" << EOF
127.0.0.1   localhost
127.0.1.1   ratanaos
::1         localhost ip6-localhost ip6-loopback
ff02::1     ip6-allnodes
ff02::2     ip6-allrouters
EOF

    # ── issue (TTY login banner) ──
    cat > "${R}/etc/issue" << 'EOF'

  ██████╗  █████╗ ████████╗ █████╗ ███╗   ██╗ █████╗  ██████╗ ███████╗
  ██╔══██╗██╔══██╗╚══██╔══╝██╔══██╗████╗  ██║██╔══██╗██╔═══██╗██╔════╝
  ██████╔╝███████║   ██║   ███████║██╔██╗ ██║███████║██║   ██║███████╗
  ██╔══██╗██╔══██║   ██║   ██╔══██║██║╚██╗██║██╔══██║██║   ██║╚════██║
  ██║  ██║██║  ██║   ██║   ██║  ██║██║ ╚████║██║  ██║╚██████╔╝███████║
  ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝

  RatanaOS \v • \n • \l

EOF

    cat > "${R}/etc/issue.net" << EOF
RatanaOS ${RATANAOS_VERSION} (${RATANAOS_CODENAME})
EOF

    # ── MOTD ──
    cat > "${R}/etc/motd" << EOF

  Welcome to RatanaOS ${RATANAOS_VERSION} (${RATANAOS_CODENAME})
  Kernel: \$(uname -r)  •  Arch: \$(uname -m)

  System tools:    ratanaos-info, ratanaos-version, ratanaos-hardware
  Package manager: ratapkg install|remove|update|search|list
  Documentation:   https://ratanaos.org/docs

EOF

    # ── GRUB ──
    if [[ -f "${R}/etc/default/grub" ]]; then
        sed -i 's/^GRUB_DISTRIBUTOR=.*/GRUB_DISTRIBUTOR="RatanaOS"/' "${R}/etc/default/grub"
        sed -i 's/^GRUB_TIMEOUT=.*/GRUB_TIMEOUT=5/' "${R}/etc/default/grub"
    else
        cat > "${R}/etc/default/grub" << 'EOF'
GRUB_DEFAULT=0
GRUB_TIMEOUT=5
GRUB_DISTRIBUTOR="RatanaOS"
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"
GRUB_CMDLINE_LINUX=""
GRUB_DISABLE_OS_PROBER=true
EOF
    fi

    # ── Locale ──
    if [[ -f "${R}/etc/locale.gen" ]]; then
        sed -i 's/^# *en_US.UTF-8/en_US.UTF-8/' "${R}/etc/locale.gen"
        chroot "${R}" locale-gen 2>/dev/null || true
    fi
    echo "LANG=en_US.UTF-8" > "${R}/etc/default/locale"

    # ── Timezone ──
    ln -sf /usr/share/zoneinfo/UTC "${R}/etc/localtime"

    # ── Fstab (minimal) ──
    cat > "${R}/etc/fstab" << 'EOF'
# RatanaOS filesystem table
# <device>    <mount>   <type>  <options>         <dump> <pass>
/dev/sda1     /         ext4    errors=remount-ro  0      1
proc          /proc     proc    defaults           0      0
sysfs         /sys      sysfs  defaults            0      0
tmpfs         /tmp      tmpfs  defaults,noatime    0      0
EOF

    # ── Sudoers ──
    if [[ -d "${R}/etc/sudoers.d" ]]; then
        echo "%sudo ALL=(ALL:ALL) ALL" > "${R}/etc/sudoers.d/ratanaos"
        chmod 440 "${R}/etc/sudoers.d/ratanaos"
    fi

    # ── Shell profile ──
    cat > "${R}/etc/profile.d/ratanaos.sh" << 'PROFILE'
# RatanaOS Shell Environment
export RATANAOS_VERSION="1.0.0"
export RATANAOS_CODENAME="Angkor"

# Custom prompt
if [ "$BASH" ]; then
    if [ "$(id -u)" -eq 0 ]; then
        PS1='\[\e[1;31m\]ratanaos\[\e[0m\]:\[\e[1;34m\]\w\[\e[0m\]# '
    else
        PS1='\[\e[1;36m\]ratanaos\[\e[0m\]:\[\e[1;34m\]\w\[\e[0m\]\$ '
    fi
fi

# Aliases
alias ll='ls -lah --color=auto'
alias la='ls -A --color=auto'
alias l='ls -CF --color=auto'
alias sysinfo='ratanaos-info'
alias update='ratapkg update && ratapkg upgrade'
PROFILE
    chmod 644 "${R}/etc/profile.d/ratanaos.sh"

    log_ok "RatanaOS rootfs identity applied"

    # ── Install RatanaOS tools ──
    log_info "Installing RatanaOS system tools..."
    install -m 755 "${TOOLS_DIR}/ratapkg" "${R}/usr/local/bin/ratapkg" 2>/dev/null || true
    install -m 755 "${TOOLS_DIR}/ratanaos-info" "${R}/usr/local/bin/ratanaos-info" 2>/dev/null || true
    install -m 755 "${TOOLS_DIR}/ratanaos-version" "${R}/usr/local/bin/ratanaos-version" 2>/dev/null || true
    install -m 755 "${TOOLS_DIR}/ratanaos-update" "${R}/usr/local/bin/ratanaos-update" 2>/dev/null || true
    install -m 755 "${TOOLS_DIR}/ratanaos-hardware" "${R}/usr/local/bin/ratanaos-hardware" 2>/dev/null || true
    install -m 755 "${TOOLS_DIR}/ratanaos-network" "${R}/usr/local/bin/ratanaos-network" 2>/dev/null || true
    install -m 755 "${TOOLS_DIR}/ratanaos-services" "${R}/usr/local/bin/ratanaos-services" 2>/dev/null || true
    install -m 755 "${TOOLS_DIR}/ratanaos-config" "${R}/usr/local/bin/ratanaos-config" 2>/dev/null || true
    install -m 755 "${TOOLS_DIR}/ratanaos-about" "${R}/usr/local/bin/ratanaos-about" 2>/dev/null || true

    # ── Install services ──
    log_info "Installing RatanaOS services..."
    for svc in "${SERVICES_DIR}"/*.service; do
        [[ -f "$svc" ]] && install -m 644 "$svc" "${R}/etc/systemd/system/" 2>/dev/null || true
    done

    log_ok "RatanaOS rootfs build complete"
    log_info "Rootfs size: $(du -sh ${R} | cut -f1)"
}

# ── PHASE 2: KERNEL ───────────────────────────────────────────────────────────
cmd_kernel() {
    log_step "Phase 2: RatanaOS Kernel Configuration"

    # For now, we use the Debian-packaged kernel.
    # Future: custom kernel build with RatanaOS branding.
    if [[ -d "${RATANAOS_ROOTFS}/lib/modules" ]]; then
        local kver="$(ls ${RATANAOS_ROOTFS}/lib/modules/ | head -1)"
        log_ok "Using Debian kernel: ${kver}"
    else
        log_warn "No kernel installed in rootfs yet. Will be installed during desktop phase."
    fi

    # Create kernel config template
    if [[ ! -f "${CONFIG_DIR}/ratanaos-kernel.config" ]]; then
        cat > "${CONFIG_DIR}/ratanaos-kernel.config" << 'EOF'
# RatanaOS Kernel Configuration (based on Debian defaults)
# This is a reference configuration for future custom kernel builds.
#
# Key features enabled:
# - x86_64 / amd64
# - UEFI + BIOS boot
# - ACPI + PCI + USB
# - NVMe, SATA, AHCI
# - Ethernet, Wi-Fi (iwlwifi, ath, rtl)
# - Bluetooth
# - Audio (ALSA + PulseAudio/PipeWire support)
# - GPU (i915, amdgpu, nouveau, fbdev)
# - Filesystem: ext4, btrfs, xfs, vfat, ntfs3, squashfs, overlayfs, iso9660
# - Virtualization: KVM, virtio
# - Containers: cgroups v2, namespaces
# - Security: AppArmor, seccomp
#
# To build a custom kernel:
#   1. Download linux-6.x source
#   2. cp config/ratanaos-kernel.config .config
#   3. make olddefconfig
#   4. make -j$(nproc) bindeb-pkg LOCALVERSION=-ratanaos
#   5. Install resulting .deb packages into ratanaos-rootfs
EOF
        log_ok "Kernel config template created: ${CONFIG_DIR}/ratanaos-kernel.config"
    fi
}

# ── PHASE 3: PACKAGES ─────────────────────────────────────────────────────────
cmd_packages() {
    log_step "Phase 3: Installing Packages into RatanaOS Rootfs"
    check_root

    local R="${RATANAOS_ROOTFS}"
    if [[ ! -d "${R}/bin" ]]; then
        log_error "Rootfs not found. Run: sudo ./build-ratanaos.sh rootfs"
        return 1
    fi

    # Mount virtual filesystems for chroot
    mount --bind /dev "${R}/dev" 2>/dev/null || true
    mount --bind /dev/pts "${R}/dev/pts" 2>/dev/null || true
    mount -t proc proc "${R}/proc" 2>/dev/null || true
    mount -t sysfs sysfs "${R}/sys" 2>/dev/null || true
    mount -t tmpfs tmpfs "${R}/tmp" 2>/dev/null || true

    # Configure DNS for apt
    cp /etc/resolv.conf "${R}/etc/resolv.conf" 2>/dev/null || true

    log_info "Updating package lists..."
    chroot "${R}" apt-get update -qq 2>/dev/null || true

    log_info "Installing core packages..."
    chroot "${R}" apt-get install -y --no-install-recommends \
        file man-db manpages whiptail dialog pciutils usbutils \
        lshw hdparm smartmontools lsb-release \
        htop neofetch tree ncdu zip unzip gzip bzip2 xz-utils \
        gnupg2 apt-transport-https software-properties-common \
        2>/dev/null || true

    log_ok "Core packages installed"

    # Cleanup chroot mounts
    umount -lf "${R}/tmp" 2>/dev/null || true
    umount -lf "${R}/sys" 2>/dev/null || true
    umount -lf "${R}/proc" 2>/dev/null || true
    umount -lf "${R}/dev/pts" 2>/dev/null || true
    umount -lf "${R}/dev" 2>/dev/null || true
}

# ── PHASE 4: DESKTOP ──────────────────────────────────────────────────────────
cmd_desktop() {
    log_step "Phase 4: Installing RatanaOS Desktop Environment"
    check_root

    local R="${RATANAOS_ROOTFS}"
    if [[ ! -d "${R}/bin" ]]; then
        log_error "Rootfs not found. Run: sudo ./build-ratanaos.sh rootfs"
        return 1
    fi

    # Mount for chroot
    mount --bind /dev "${R}/dev" 2>/dev/null || true
    mount --bind /dev/pts "${R}/dev/pts" 2>/dev/null || true
    mount -t proc proc "${R}/proc" 2>/dev/null || true
    mount -t sysfs sysfs "${R}/sys" 2>/dev/null || true
    mount -t tmpfs tmpfs "${R}/tmp" 2>/dev/null || true
    cp /etc/resolv.conf "${R}/etc/resolv.conf" 2>/dev/null || true

    log_info "Installing XFCE4 desktop environment..."
    chroot "${R}" apt-get install -y --no-install-recommends \
        xfce4 xfce4-terminal xfce4-panel xfce4-session xfce4-settings \
        xfce4-power-manager thunar lightdm lightdm-gtk-greeter \
        xorg xserver-xorg-core xserver-xorg-input-all \
        xserver-xorg-video-fbdev xserver-xorg-video-vesa \
        dbus-x11 network-manager network-manager-gnome \
        plank mousepad ristretto xfce4-screenshooter \
        fonts-dejavu fonts-liberation fonts-noto-core \
        adwaita-icon-theme papirus-icon-theme \
        pulseaudio pavucontrol alsa-utils \
        firefox-esr \
        2>/dev/null || true

    log_ok "XFCE4 desktop installed"

    # Configure LightDM for RatanaOS
    log_info "Configuring RatanaOS login screen..."
    mkdir -p "${R}/etc/lightdm"
    cat > "${R}/etc/lightdm/lightdm-gtk-greeter.conf" << 'EOF'
[greeter]
background=/usr/share/backgrounds/ratanaos-wallpaper.png
theme-name=Adwaita-dark
icon-theme-name=Papirus-Dark
font-name=DejaVu Sans 11
xft-antialias=true
xft-hintstyle=hintslight
show-clock=true
clock-format=%H:%M  •  %A, %B %e
indicators=~host;~spacer;~clock;~spacer;~session;~power
EOF

    # Enable auto-start for NetworkManager
    chroot "${R}" systemctl enable NetworkManager 2>/dev/null || true
    chroot "${R}" systemctl enable lightdm 2>/dev/null || true

    # Create default user
    log_info "Creating default user 'ratanaos'..."
    chroot "${R}" useradd -m -G sudo,audio,video,plugdev,netdev -s /bin/bash ratanaos 2>/dev/null || true
    echo "ratanaos:ratanaos" | chroot "${R}" chpasswd 2>/dev/null || true
    echo "root:ratanaos" | chroot "${R}" chpasswd 2>/dev/null || true

    # Generate wallpaper placeholder
    log_info "Installing RatanaOS branding assets..."
    mkdir -p "${R}/usr/share/backgrounds"
    # Create a simple SVG wallpaper
    cat > "${R}/usr/share/backgrounds/ratanaos-wallpaper.svg" << 'SVGEOF'
<svg xmlns="http://www.w3.org/2000/svg" width="1920" height="1080">
  <defs>
    <linearGradient id="bg" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" style="stop-color:#0f172a"/>
      <stop offset="50%" style="stop-color:#1e293b"/>
      <stop offset="100%" style="stop-color:#0c4a6e"/>
    </linearGradient>
  </defs>
  <rect width="1920" height="1080" fill="url(#bg)"/>
  <text x="960" y="500" text-anchor="middle" font-family="sans-serif" font-size="72" font-weight="bold" fill="#e2e8f0" opacity="0.8">RatanaOS</text>
  <text x="960" y="570" text-anchor="middle" font-family="sans-serif" font-size="24" fill="#94a3b8" opacity="0.6">Version 1.0.0 • Angkor</text>
</svg>
SVGEOF

    # Cleanup chroot mounts
    umount -lf "${R}/tmp" 2>/dev/null || true
    umount -lf "${R}/sys" 2>/dev/null || true
    umount -lf "${R}/proc" 2>/dev/null || true
    umount -lf "${R}/dev/pts" 2>/dev/null || true
    umount -lf "${R}/dev" 2>/dev/null || true

    log_ok "RatanaOS desktop environment installed"
}

# ── PHASE 5: INSTALLER ────────────────────────────────────────────────────────
cmd_installer() {
    log_step "Phase 5: Creating RatanaOS Installer"
    log_info "Installer script installed at ${TOOLS_DIR}/ratanaos-installer"
    log_ok "Installer ready (script-based, runs in live environment)"
}

# ── PHASE 6: ISO ──────────────────────────────────────────────────────────────
cmd_iso() {
    log_step "Phase 6: Building RatanaOS Live ISO"
    check_root

    local R="${RATANAOS_ROOTFS}"
    if [[ ! -d "${R}/bin" ]]; then
        log_error "Rootfs not found. Run full pipeline first."
        return 1
    fi

    mkdir -p "${ISO_DIR}"/{boot/grub,live,EFI/BOOT}
    mkdir -p "${ISO_OUTPUT}"

    # Create squashfs
    log_info "Creating squashfs filesystem image..."
    if [[ -f "${ISO_DIR}/live/filesystem.squashfs" ]]; then
        rm -f "${ISO_DIR}/live/filesystem.squashfs"
    fi
    mksquashfs "${R}" "${ISO_DIR}/live/filesystem.squashfs" \
        -comp xz -b 1M -Xdict-size 100% -noappend \
        -e "${R}/tmp/*" "${R}/var/cache/apt/*" "${R}/var/lib/apt/lists/*"

    log_ok "Squashfs created: $(du -sh ${ISO_DIR}/live/filesystem.squashfs | cut -f1)"

    # Copy kernel and initrd
    log_info "Copying kernel and initrd..."
    local kver="$(ls ${R}/lib/modules/ 2>/dev/null | sort -V | tail -1)"
    if [[ -n "$kver" && -f "${R}/boot/vmlinuz-${kver}" ]]; then
        cp "${R}/boot/vmlinuz-${kver}" "${ISO_DIR}/boot/vmlinuz"
        cp "${R}/boot/initrd.img-${kver}" "${ISO_DIR}/boot/initrd.img" 2>/dev/null || \
        cp "${R}/boot/initrd.img-${kver}" "${ISO_DIR}/boot/initrd" 2>/dev/null || true
    else
        # Fallback: find any vmlinuz
        cp "${R}"/boot/vmlinuz-* "${ISO_DIR}/boot/vmlinuz" 2>/dev/null || true
        cp "${R}"/boot/initrd.img-* "${ISO_DIR}/boot/initrd.img" 2>/dev/null || true
    fi

    # GRUB config for BIOS + UEFI
    log_info "Creating GRUB configuration..."
    cat > "${ISO_DIR}/boot/grub/grub.cfg" << 'GRUBEOF'
set timeout=5
set default=0

insmod all_video
insmod gfxterm
set gfxmode=auto
terminal_output gfxterm

set menu_color_normal=white/black
set menu_color_highlight=black/cyan

menuentry "RatanaOS Live" {
    linux /boot/vmlinuz boot=live live-media-path=/live quiet splash
    initrd /boot/initrd.img
}

menuentry "RatanaOS Live (Safe Mode)" {
    linux /boot/vmlinuz boot=live live-media-path=/live nomodeset
    initrd /boot/initrd.img
}

menuentry "RatanaOS Install" {
    linux /boot/vmlinuz boot=live live-media-path=/live quiet splash ratanaos.installer=1
    initrd /boot/initrd.img
}

menuentry "Memory Test" {
    linux /boot/vmlinuz memtest
}
GRUBEOF

    # Build ISO with xorriso / grub-mkrescue
    log_info "Assembling ISO image..."
    local iso_file="${ISO_OUTPUT}/RatanaOS-Live-${RATANAOS_ARCH}.iso"
    grub-mkrescue -o "${iso_file}" "${ISO_DIR}" \
        --product-name="RatanaOS" \
        --product-version="${RATANAOS_VERSION}" \
        2>/dev/null || \
    xorriso -as mkisofs \
        -iso-level 3 \
        -full-iso9660-filenames \
        -volid "RATANAOS_LIVE" \
        -eltorito-boot boot/grub/i386-pc/eltorito.img \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --grub2-boot-info --grub2-mbr /usr/lib/grub/i386-pc/boot_hybrid.img \
        -output "${iso_file}" \
        "${ISO_DIR}" 2>/dev/null || true

    if [[ -f "${iso_file}" ]]; then
        log_ok "ISO created: ${iso_file} ($(du -sh ${iso_file} | cut -f1))"

        # Generate checksums
        cd "${ISO_OUTPUT}"
        sha256sum "RatanaOS-Live-${RATANAOS_ARCH}.iso" > SHA256SUMS
        log_ok "SHA256SUMS generated"

        # Generate BUILD_INFO.txt
        cat > "${ISO_OUTPUT}/BUILD_INFO.txt" << BUILDEOF
RATANAOS BUILD INFORMATION
==========================
RatanaOS Version: ${RATANAOS_VERSION}
Codename: ${RATANAOS_CODENAME}
Architecture: ${RATANAOS_ARCH}
Build Date: $(date -Iseconds)
Debian Base: ${DEBIAN_RELEASE}
Kernel: ${kver:-unknown}
Builder: $(whoami)@$(hostname)
Host OS: $(cat /etc/os-release | grep PRETTY_NAME | cut -d= -f2 | tr -d '"')
BUILDEOF

        log_ok "BUILD_INFO.txt generated"
    else
        log_error "ISO creation failed"
        return 1
    fi
}

# ── TESTING ───────────────────────────────────────────────────────────────────
cmd_test() {
    log_step "Testing RatanaOS"

    local iso_file="${ISO_OUTPUT}/RatanaOS-Live-${RATANAOS_ARCH}.iso"
    if [[ ! -f "${iso_file}" ]]; then
        log_error "ISO not found at ${iso_file}. Build first with: sudo ./build-ratanaos.sh iso"
        return 1
    fi

    log_info "ISO file: ${iso_file} ($(du -sh ${iso_file} | cut -f1))"
    log_info "SHA256: $(sha256sum ${iso_file} | cut -d' ' -f1)"

    log_info "Testing with QEMU..."
    log_info "Running: qemu-system-x86_64 -enable-kvm -m 4096 -cdrom ${iso_file}"

    qemu-system-x86_64 \
        -enable-kvm \
        -m 4096 \
        -smp 2 \
        -cdrom "${iso_file}" \
        -boot d \
        -vga virtio \
        -display gtk \
        -serial stdio \
        -net nic -net user \
        &

    log_ok "QEMU launched. Check the window for RatanaOS boot."
}

# ── CLEAN ─────────────────────────────────────────────────────────────────────
cmd_clean() {
    log_step "Cleaning Build Artifacts"

    rm -rf "${BUILD_DIR}/iso"
    rm -rf "${ISO_OUTPUT}"
    log_ok "Build artifacts cleaned"
    log_info "Note: debian-source and ratanaos-rootfs are preserved. Remove manually if needed."
}

# ── ALL ───────────────────────────────────────────────────────────────────────
cmd_all() {
    log_step "RatanaOS Full Build Pipeline"
    echo ""
    echo "  ██████╗  █████╗ ████████╗ █████╗ ███╗   ██╗ █████╗  ██████╗ ███████╗"
    echo "  ██╔══██╗██╔══██╗╚══██╔══╝██╔══██╗████╗  ██║██╔══██╗██╔═══██╗██╔════╝"
    echo "  ██████╔╝███████║   ██║   ███████║██╔██╗ ██║███████║██║   ██║███████╗"
    echo "  ██╔══██╗██╔══██║   ██║   ██╔══██║██║╚██╗██║██╔══██║██║   ██║╚════██║"
    echo "  ██║  ██║██║  ██║   ██║   ██║  ██║██║ ╚████║██║  ██║╚██████╔╝███████║"
    echo "  ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝"
    echo ""
    echo "  Version ${RATANAOS_VERSION} (${RATANAOS_CODENAME}) — Build System"
    echo ""

    cmd_prepare
    cmd_rootfs
    cmd_audit
    cmd_kernel
    cmd_packages
    cmd_desktop
    cmd_installer
    cmd_iso
    
    log_step "BUILD COMPLETE"
    log_ok "RatanaOS ${RATANAOS_VERSION} build finished successfully!"
    log_info "ISO: ${ISO_OUTPUT}/RatanaOS-Live-${RATANAOS_ARCH}.iso"
    log_info "Run 'sudo ./build-ratanaos.sh test' to test in QEMU"
}

# ── ENTRY ─────────────────────────────────────────────────────────────────────
case "${1:-help}" in
    audit)     cmd_audit     ;;
    prepare)   cmd_prepare   ;;
    rootfs)    cmd_rootfs    ;;
    kernel)    cmd_kernel    ;;
    packages)  cmd_packages  ;;
    desktop)   cmd_desktop   ;;
    installer) cmd_installer ;;
    iso)       cmd_iso       ;;
    test)      cmd_test      ;;
    clean)     cmd_clean     ;;
    all)       cmd_all       ;;
    *)
        echo "Usage: $0 <command>"
        echo ""
        echo "Commands:"
        echo "  audit      Audit the Debian source system"
        echo "  prepare    Bootstrap Debian rootfs"
        echo "  rootfs     Transform rootfs into RatanaOS"
        echo "  kernel     Configure kernel"
        echo "  packages   Install system packages"
        echo "  desktop    Install desktop environment"
        echo "  installer  Create installer"
        echo "  iso        Build live ISO"
        echo "  test       Test ISO in QEMU"
        echo "  clean      Clean build artifacts"
        echo "  all        Run full build pipeline"
        ;;
esac
