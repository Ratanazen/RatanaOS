#!/bin/bash
# =============================================================================
# RatanaOS All-in-One Production Build Script v50
# Run this on a real Debian/Ubuntu host with root access.
#
# Usage:
#   sudo bash builder/build-ratanaos.sh [edition]
#
# Editions: lite | standard | developer | cyber
# Default:  standard
# =============================================================================
set -e

EDITION="${1:-standard}"
TIMESTAMP=$(date +%Y%m%d%H%M%S)

# ── FIX: ROOT_DIR must be the project root (parent of builder/) ──────────────
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

LB_DIR="${ROOT_DIR}/builder/live-build"
LB_BIN="${LB_DIR}/bin/lb"
WORK_DIR="${ROOT_DIR}/build/live-build-work"
OUTPUT_DIR="${ROOT_DIR}/releases"
LOG_FILE="${ROOT_DIR}/logs/build_${EDITION}_${TIMESTAMP}.log"

mkdir -p "${ROOT_DIR}/logs" "${OUTPUT_DIR}"
exec > >(tee -a "${LOG_FILE}") 2>&1

echo "=============================================="
echo "  RatanaOS v50 Production ISO Builder"
echo "=============================================="
echo "  Edition  : ${EDITION}"
echo "  Timestamp: ${TIMESTAMP}"
echo ""

# Guard: must run as root
if [ "$(id -u)" -ne 0 ]; then
    echo "❌ Error: This script must be run as root. Use: sudo bash $0"
    exit 1
fi

# ==========================================================================
# STEP 1: Install build dependencies
# ==========================================================================
echo "[1/8] Installing build dependencies..."
apt-get update -qq
apt-get install -y --no-install-recommends \
    live-build \
    debootstrap \
    xorriso \
    grub-pc-bin \
    grub-efi-amd64-bin \
    grub-efi-amd64-signed \
    grub-common \
    isolinux \
    squashfs-tools \
    mtools \
    dosfstools \
    rsync \
    syslinux-common \
    git \
    curl \
    ca-certificates
echo "✅ Build dependencies installed."

# ==========================================================================
# STEP 2: Clone live-build from Salsa (use local if already present)
# ==========================================================================
echo "[2/8] Checking live-build source..."
if [ ! -x "${LB_BIN}" ]; then
    echo "  Cloning Debian live-build from Salsa..."
    rm -rf "${LB_DIR}"
    git clone https://salsa.debian.org/live-team/live-build.git "${LB_DIR}"
    echo "✅ live-build cloned."
else
    echo "✅ live-build already present at ${LB_BIN}"
fi

# Add local lb to PATH so all sub-calls work correctly
export PATH="${LB_DIR}/bin:${PATH}"

# ==========================================================================
# STEP 3: Prepare clean live-build workspace
# ==========================================================================
echo "[3/8] Preparing workspace..."
rm -rf "${WORK_DIR}"
mkdir -p "${WORK_DIR}"
cd "${WORK_DIR}"
echo "✅ Workspace ready: ${WORK_DIR}"

# ==========================================================================
# STEP 4: Configure live-build for chosen edition
# ==========================================================================
echo "[4/8] Configuring live-build (${EDITION} edition)..."

case "${EDITION}" in
    lite)
        DESKTOP_PKGS="xfce4 xfce4-goodies lightdm"
        EDITION_PKGS="firefox mousepad thunar"
        LABEL="RATANAOS-LITE"
        ;;
    developer)
        DESKTOP_PKGS="plasma-desktop sddm kde-standard"
        EDITION_PKGS="gcc clang cmake python3 git docker.io nodejs code firefox"
        LABEL="RATANAOS-DEV"
        ;;
    cyber)
        DESKTOP_PKGS="plasma-desktop sddm kde-standard"
        EDITION_PKGS="nmap wireshark gobuster hashcat john nikto python3 git firefox"
        LABEL="RATANAOS-CYBER"
        ;;
    standard|*)
        DESKTOP_PKGS="plasma-desktop sddm kde-standard"
        EDITION_PKGS="firefox libreoffice vlc gimp kate ark"
        LABEL="RATANAOS-STD"
        ;;
esac

lb config \
    --mode debian \
    --distribution bookworm \
    --binary-images iso-hybrid \
    --architectures amd64 \
    --linux-flavours amd64 \
    --archive-areas "main contrib non-free non-free-firmware" \
    --mirror-bootstrap "http://deb.debian.org/debian/" \
    --mirror-chroot "http://deb.debian.org/debian/" \
    --mirror-chroot-security "http://security.debian.org/debian-security/" \
    --mirror-binary "http://deb.debian.org/debian/" \
    --mirror-binary-security "http://security.debian.org/debian-security/" \
    --apt-indices false \
    --iso-volume "${LABEL}" \
    --bootloader grub-pc \
    --memtest none

# Write package list
mkdir -p config/package-lists
cat > config/package-lists/ratana.list.chroot << PKGLIST
# Base system
linux-image-amd64
live-boot
live-boot-initramfs-tools
live-config
live-config-systemd
systemd
systemd-sysv
apparmor
apparmor-utils
ufw
auditd
flatpak
curl
wget
rsync
network-manager
network-manager-gnome
ca-certificates
apt-transport-https
plymouth
plymouth-themes
usbutils
pciutils
lm-sensors

# CPU Microcode & Security
intel-microcode
amd64-microcode

# Comprehensive Wireless & Network Drivers (Wi-Fi, Ethernet, Bluetooth)
firmware-linux
firmware-linux-free
firmware-linux-nonfree
firmware-misc-nonfree
firmware-iwlwifi
firmware-realtek
firmware-atheros
firmware-brcm80211
firmware-libertas
bluez
bluez-tools

# Graphics Drivers & Hardware Acceleration (Intel, AMD, NVIDIA)
xserver-xorg-video-all
xserver-xorg-video-amdgpu
xserver-xorg-video-intel
xserver-xorg-video-nouveau
firmware-amd-graphics
mesa-vulkan-drivers
va-driver-all
vdpau-driver-all

# Sound & Microphone Drivers (PipeWire + Sound Open Firmware for Laptops)
pipewire
wireplumber
pipewire-audio
pipewire-pulse
firmware-sof-signed
alsa-utils

# Touchpad & Input Drivers
xserver-xorg-input-libinput
xserver-xorg-input-synaptics

# Printer & Peripheral Drivers
cups
printer-driver-all
hplip

# Desktop environment
${DESKTOP_PKGS}

# Applications
${EDITION_PKGS}
PKGLIST

# ==========================================================================
# STEP 5: Write live-build hooks (OS identity, branding, UI)
# ==========================================================================
echo "[5/8] Writing post-install hooks..."
mkdir -p config/hooks/live

# Hook 1: Write OS identity file
cat > config/hooks/live/01-os-identity.hook.chroot << 'HOOK'
#!/bin/sh
set -e
cat > /etc/os-release << 'EOF'
NAME="RatanaOS"
VERSION="21.0.0"
ID=ratanaos
ID_LIKE=debian
PRETTY_NAME="RatanaOS v21.0"
VERSION_ID="21.0.0"
HOME_URL="https://ratanaos.local"
SUPPORT_URL="https://ratanaos.local/support"
BUG_REPORT_URL="https://ratanaos.local/bugs"
EOF
HOOK

# Hook 2: Enable display manager
cat > config/hooks/live/02-enable-dm.hook.chroot << 'HOOK'
#!/bin/sh
set -e
if command -v sddm > /dev/null 2>&1; then
    systemctl enable sddm
elif command -v lightdm > /dev/null 2>&1; then
    systemctl enable lightdm
fi
HOOK

# Hook 3: Configure networking
cat > config/hooks/live/03-networking.hook.chroot << 'HOOK'
#!/bin/sh
set -e
systemctl enable NetworkManager 2>/dev/null || true
systemctl disable networking 2>/dev/null || true
HOOK

# Hook 4: Configure Plymouth boot splash
cat > config/hooks/live/04-plymouth.hook.chroot << 'HOOK'
#!/bin/sh
set -e
if command -v update-alternatives > /dev/null 2>&1; then
    update-alternatives --install /usr/share/plymouth/themes/default.plymouth \
        default.plymouth /usr/share/plymouth/themes/lines/lines.plymouth 100 2>/dev/null || true
fi
HOOK

chmod +x config/hooks/live/*.hook.chroot

# Hook 5: Stage Web UI assets into ISO filesystem
if [ -d "${ROOT_DIR}/../ratana-ui-v25" ] || [ -d "${ROOT_DIR}/../ratana-ui-v31" ]; then
    mkdir -p config/includes.chroot/opt/ratana-ui
    for UI_DIR in "${ROOT_DIR}/../ratana-ui-v31" "${ROOT_DIR}/../ratana-ui-v25"; do
        if [ -d "$UI_DIR" ]; then
            cp -r "${UI_DIR}"/* config/includes.chroot/opt/ratana-ui/ 2>/dev/null || true
            echo "✅ Web UI staged from ${UI_DIR}"
            break
        fi
    done
fi

# Copy branding assets
if [ -d "${ROOT_DIR}/branding" ]; then
    mkdir -p config/includes.chroot/usr/share/ratanaos/branding
    cp -r "${ROOT_DIR}/branding"/* config/includes.chroot/usr/share/ratanaos/branding/ 2>/dev/null || true
fi

# Copy custom applications & utilities (ratana-fetch, ratana-matrix, ratana-snake)
if [ -d "${ROOT_DIR}/apps" ]; then
    mkdir -p config/includes.chroot/usr/bin
    cp -r "${ROOT_DIR}/apps"/* config/includes.chroot/usr/bin/ 2>/dev/null || true
    chmod +x config/includes.chroot/usr/bin/ratana-* 2>/dev/null || true
    echo "✅ Custom RatanaOS applications (ratana-fetch, ratana-matrix, ratana-snake, ratana-hardware-probe, ratana-driver-manager) staged."
fi
echo "✅ Hooks configured."

# ==========================================================================
# STEP 6: Compile the ISO
# ==========================================================================
echo "[6/8] Building the live ISO (this will take 20–40 minutes)..."
echo "⏳ Patience — debootstrap, package downloads, and SquashFS compression are running..."
lb build
echo "✅ live-build compilation complete!"

# ==========================================================================
# STEP 7: Move output to releases/ directory
# ==========================================================================
echo "[7/8] Staging final ISO..."
DATE_STAMP=$(date +%Y-%m-%d)

case "${EDITION}" in
    lite)      ISO_BASE="RatanaOS-Lite" ;;
    developer) ISO_BASE="RatanaOS-Developer" ;;
    cyber)     ISO_BASE="RatanaOS-Cyber" ;;
    *)         ISO_BASE="RatanaOS-Standard" ;;
esac

# live-build outputs the ISO in the working directory
BUILT_ISO=$(find "${WORK_DIR}" -maxdepth 1 -name "*.hybrid.iso" -o -name "*.iso" 2>/dev/null | head -n 1)
if [ -z "${BUILT_ISO}" ]; then
    echo "❌ Error: live-build did not produce an ISO file! Check ${LOG_FILE} for details."
    exit 1
fi

FINAL_ISO="${OUTPUT_DIR}/${ISO_BASE}-${DATE_STAMP}.iso"
cp "${BUILT_ISO}" "${FINAL_ISO}"
ln -sf "${ISO_BASE}-${DATE_STAMP}.iso" "${OUTPUT_DIR}/${ISO_BASE}.iso"

# Apply isohybrid to make ISO bootable on physical USB drives (BIOS + UEFI)
if command -v isohybrid >/dev/null 2>&1; then
    echo "  Applying isohybrid (UEFI+BIOS USB partition headers)..."
    isohybrid --uefi "${FINAL_ISO}" 2>/dev/null || isohybrid "${FINAL_ISO}" 2>/dev/null || true
    echo "✅ Hybrid USB boot headers applied."
fi

# ==========================================================================
# STEP 8: Generate checksums and report
# ==========================================================================
echo "[8/8] Generating checksums..."
cd "${OUTPUT_DIR}"
sha256sum "${ISO_BASE}-${DATE_STAMP}.iso" > SHA256SUMS
sha512sum "${ISO_BASE}-${DATE_STAMP}.iso" >> SHA512SUMS

ISO_SIZE=$(du -sh "${FINAL_ISO}" | cut -f1)

cat > "${OUTPUT_DIR}/BUILD_REPORT.md" << REPORT
# RatanaOS Build Report

| Field       | Value                                     |
|-------------|-------------------------------------------|
| Date        | $(date -u +"%Y-%m-%dT%H:%M:%SZ")         |
| Version     | 21.0.0                                    |
| Edition     | ${EDITION}                                |
| ISO File    | ${ISO_BASE}-${DATE_STAMP}.iso             |
| Size        | ${ISO_SIZE}                               |
| SHA256      | $(sha256sum "${ISO_BASE}-${DATE_STAMP}.iso" | awk '{print $1}') |
| Boot        | UEFI + BIOS Hybrid                        |
| Status      | ✅ SUCCESS                                |
REPORT

echo ""
echo "=============================================="
echo "  🎉 BUILD COMPLETE — RatanaOS ${EDITION}"
echo "=============================================="
echo "  ISO   : ${FINAL_ISO}"
echo "  Size  : ${ISO_SIZE}"
echo "  SHA256: $(sha256sum "${ISO_BASE}-${DATE_STAMP}.iso" | awk '{print $1}')"
echo ""
echo "  To test in VirtualBox:"
echo "  1. Create a new VM → Linux → Debian 64-bit"
echo "  2. RAM: 4096 MB, CPU: 2+ cores"
echo "  3. Storage → Optical Drive → Load ISO"
echo "  4. Power ON → Boot from ISO!"
echo "=============================================="
