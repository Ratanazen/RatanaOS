#!/usr/bin/env bash
# ==============================================================================
# RatanaOS Installer: GRUB Bootloader Installation Module (UEFI / BIOS)
# ==============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/install-system.sh"

install_bootloader() {
    local dev="$1"
    local target_mount="${2:-/mnt/target}"
    local mode="${3:-auto}"

    if [ "$mode" = "auto" ]; then
        if [ -d "/sys/firmware/efi" ]; then
            mode="uefi"
        else
            mode="bios"
        fi
    fi

    echo "==> Installing GRUB bootloader on $dev ($mode mode)..."
    bind_chroot "$target_mount"

    # Configure /etc/default/grub for RatanaOS
    cat << 'GEOF' > "${target_mount}/etc/default/grub"
GRUB_DEFAULT=0
GRUB_TIMEOUT=5
GRUB_DISTRIBUTOR="RatanaOS"
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"
GRUB_CMDLINE_LINUX=""
GRUB_GFXMODE=auto
GRUB_THEME=""
GEOF

    if [ "$mode" = "uefi" ]; then
        echo "Executing grub-install for x86_64-efi..."
        chroot "$target_mount" grub-install \
            --target=x86_64-efi \
            --efi-directory=/boot/efi \
            --bootloader-id=RatanaOS \
            --recheck \
            --no-nvram

        # Provide UEFI Fallback loader (BOOTX64.EFI)
        mkdir -p "${target_mount}/boot/efi/EFI/BOOT"
        if [ -f "${target_mount}/boot/efi/EFI/RatanaOS/grubx64.efi" ]; then
            cp -f "${target_mount}/boot/efi/EFI/RatanaOS/grubx64.efi" "${target_mount}/boot/efi/EFI/BOOT/BOOTX64.EFI"
        fi
    else
        echo "Executing grub-install for BIOS on $dev..."
        chroot "$target_mount" grub-install \
            --target=i386-pc \
            --recheck \
            "$dev"
    fi

    echo "Generating /boot/grub/grub.cfg..."
    chroot "$target_mount" update-grub

    unbind_chroot "$target_mount"

    # Verification
    echo "Verifying boot assets..."
    local kernel_count initrd_count
    kernel_count=$(ls -1 "${target_mount}/boot"/vmlinuz-* 2>/dev/null | wc -l)
    initrd_count=$(ls -1 "${target_mount}/boot"/initrd.img-* 2>/dev/null | wc -l)

    if [ "$kernel_count" -gt 0 ] && [ "$initrd_count" -gt 0 ] && [ -f "${target_mount}/boot/grub/grub.cfg" ]; then
        echo "Verification PASSED: Found $kernel_count kernel(s), $initrd_count initrd(s), and valid grub.cfg."
    else
        echo "Verification WARNING: Kernel, initrd, or grub.cfg not fully found." >&2
    fi
}

if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    if [ $# -lt 1 ]; then
        echo "Usage: $0 <target-disk> [target-mount] [auto|uefi|bios]"
        exit 1
    fi
    install_bootloader "$1" "${2:-/mnt/target}" "${3:-auto}"
fi
