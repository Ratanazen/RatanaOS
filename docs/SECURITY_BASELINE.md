# 🛡️ RatanaOS Security & Hardening Baseline (Phase 5)

This document specifies the security policies, configuration defaults, and rationale for RatanaOS 1.0 (Debian Bookworm base).

---

## 🔒 Baseline Checklist & Configurations

### 1. Firewall (ufw / nftables)
- **Policy**: In dev/workstation live mode, standard Linux filtering is unblocked for local developer tools (e.g. docker, local test servers, node, python). For production installs, `ufw` is available with default incoming deny, outgoing allow.
- **Packages**: `ufw`, `iptables`, `nftables` available via APT.

### 2. Password & Credential Security
- **Root Account**: Root password login is locked by default (`!`). Administrative privileges are managed exclusively via `sudo` with group membership in `sudo`.
- **Live User**: Live demo user `user` operates with passwordless sudo solely within the volatile live ramdisk session.
- **Calamares Installed System**: Calamares installer explicitly requires the user to define a non-empty password for their personal account and optionally an independent root password before installation commences. Blank passwords are fundamentally rejected by Calamares validation.

### 3. Network Surface & Remote Access
- **SSH Daemon**: `openssh-server` is **NOT** installed by default. Port 22 is closed on cold boot. Users requiring SSH access can install and enable it explicitly via `sudo apt install openssh-server && sudo systemctl enable --now ssh`.
- **Open Ports**: Zero listening network ports out of the box (verified by `ss -tulpn`).

### 4. Mandatory Access Control (AppArmor)
- **Status**: Enabled by default in stock Debian Bookworm Linux kernel.
- **Policy**: RatanaOS preserves standard Debian AppArmor profiles without relaxation. Default profiles guard critical userspace utilities and network daemons.

### 5. Automatic Security Updates
- **Configuration**: Debian's `unattended-upgrades` package is supported. Security repository `bookworm-security` is pre-configured in `/etc/apt/sources.list` to receive upstream CVE patches without delay.

---

## 🧪 Verification Commands
```bash
# Verify no listening services
ss -tulpn

# Verify AppArmor status
systemctl status apparmor
aa-status

# Verify sudo privileges
sudo -l
```

---

## 🔐 LUKS Full-Disk Encryption (Phase 9)
- **Availability**: Enabled as an opt-in option in the Calamares installer
  partition module. Users select "Encrypt system" during installation.
- **Implementation**: Standard Debian `cryptsetup` + `cryptsetup-initramfs`.
  The initramfs prompts for the LUKS passphrase at boot.
- **Default**: Encryption is NOT forced — it is presented as a checkbox
  during install, matching standard Debian/Ubuntu installer behavior.

## 🔑 UEFI Secure Boot (Phase 9)
- **Chain**: Debian's `shim-signed` → `grub-efi-amd64-signed` → Linux kernel.
- **RatanaOS GRUB Theme**: Theme files under `/boot/grub/themes/ratanaos/`
  are non-executable data assets (images, fonts, text config). They do NOT
  break the Secure Boot signature chain — only executable bootloader code
  is verified by shim/GRUB, not theme resources. **Confirmed safe.**
- **Hybrid Boot**: The ISO image is built with both `syslinux` (BIOS/Legacy)
  and `grub-efi` (UEFI) bootloaders, supporting cold boot on both
  firmware types.
- **Test Commands**:
  \`\`\`bash
  # UEFI boot test (requires OVMF firmware)
  qemu-system-x86_64 -bios /usr/share/OVMF/OVMF_CODE.fd \\
    -cdrom build/ratanaos-live-amd64.hybrid.iso -m 2048 -smp 2

  # Legacy BIOS boot test
  qemu-system-x86_64 \\
    -cdrom build/ratanaos-live-amd64.hybrid.iso -m 2048 -smp 2
  \`\`\`
