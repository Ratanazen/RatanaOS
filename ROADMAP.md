# RiOS — Master Full Tasks & Engineering Roadmap (v1.0.0 — 100% Comprehensive)

## Phase 1: Operating System Core & Debian 12 Base
- `[x]` Establish Debian 12 Bookworm amd64 as pure distribution foundation
- `[x]` Hybrid BIOS + UEFI bootloader architecture (Syslinux + GRUB-EFI)
- `[x]` Modern multi-threaded ZSTD compression engine (`fast-live-build.sh`)
- `[x]` Update system identity in `/etc/os-release` and `issue`
- `[x]` Comprehensive hardware firmware (`non-free-firmware`, intel/amd microcode)
- `[x]` Clean Plymouth boot animation and quiet kernel splash

## Phase 2: Package Ecosystem & Profile Suites
- `[x]` **Cybersecurity Suite** (`rios-cyber.list.chroot`): 76+ verified penetration testing tools (Nmap, Wireshark, Hashcat, Bettercap, Sublist3r, Wifite)
- `[x]` **Developer Suite** (`rios-dev.list.chroot`): GCC, Make, Python3, Node, Rust/Cargo, Go, Neovim, Tmux, Git, Podman
- `[x]` **Server Infrastructure Suite** (`rios-server.list.chroot`): OpenSSH, Nginx, Apache2, UFW, Fail2ban, WireGuard, Cockpit
- `[x]` **Desktop Compositors**: Hyprland (Wayland), Sway (fallback), GNOME 43, KDE Plasma 5
- `[x]` **Hardware & Drivers**: Mesa, Vulkan, ALSA/PipeWire, Bluetooth, NetworkManager

## Phase 3: Real Computer Installer Engine (`ri-installer`)
- `[x]` Non-destructive disk detection & live medium exclusion (`disk.sh`)
- `[x]` Automated & Interactive Partitioning: ESP FAT32 + Root EXT4 (`partition.sh`)
- `[x]` Filesystem formatting with partition UUID detection (`filesystem.sh`)
- `[x]` System deployment from SquashFS rootfs or live synchronization (`install-system.sh`)
- `[x]` Bootloader deployment: GRUB EFI + MBR BIOS with fallback recovery (`bootloader.sh`)
- `[x]` Dynamic `/etc/fstab` generation by filesystem UUIDs (`filesystem.sh`)
- `[x]` User provisioning, sudo permissions, and locale configuration (`user.sh`, `network.sh`)
- `[x]` Support for Server profile installation (`--session server` / `--profile server`)
- `[x]` Clean unmount and target sync verification (`finish.sh`)

## Phase 4: Server Virtualization & Cloud Runner
- `[x]` Dedicated QEMU KVM Server Runner (`scripts/run_server.sh`)
- `[x]` Automated port forward mappings:
  - Port `2222 -> 22` (SSH)
  - Port `8080 -> 80` (HTTP Web Server)
  - Port `8443 -> 443` (HTTPS)
  - Port `9090 -> 9090` (Cockpit Web Console)
- `[x]` Support for headless, curses terminal, and GUI display modes
- `[x]` Support booting from Live ISO or installed virtual disk (`--disk`)

## Phase 5: Server Administration & Management CLI (`ri-server`)
- `[x]` Daemon status inspection (`ri-server status`)
- `[x]` Host resource metrics & load monitoring (`ri-server info`)
- `[x]` Active listening port scanner (`ri-server ports`)
- `[x]` Service lifecycle manager (`start`, `stop`, `restart`, `enable`, `disable`)
- `[x]` UFW firewall management (`status`, `enable`, `allow <port>`)
- `[x]` Cockpit Web Console launcher (`ri-server cockpit start`)
- `[x]` Installed into live chroot `/usr/local/bin/ri-server`

## Phase 6: Custom & Modular ISO Build System
- `[x]` Fast Live-Build Runner with Docker isolation (`fast-live-build.sh`, `docker-live-build.sh`)
- `[x]` Fix wildcard copy bug for multi-ISO builds
- `[x]` Modular Custom Builder (`scripts/build_custom.sh`) with profile flags:
  - Preset: `all` (Full edition with all suites)
  - Preset: `server` (Dedicated server edition)
  - Preset: `cyber` (Pen-testing station)
  - Preset: `dev` (Developer workstation)
  - Preset: `minimal` (Minimal Wayland base)
- `[x]` Master build pipeline (`./build.sh [all|server|cyber|dev|custom|status|clean]`)
- `[x]` Full system master build pipeline (`scripts/build_full_system.sh`)
- `[x]` Debian Security & CVE patch archive integrated into build (`security.list.chroot`)
- `[x]` Profile inspection tool (`./build.sh status`)

## Phase 7: System Diagnostics & Self-Healing (`ri-doctor`)
- `[x]` Audit Desktop Compositors (Hyprland, Sway, GNOME, KDE)
- `[x]` Audit Server Daemons (SSH, Nginx, Docker, UFW, Fail2ban, WireGuard, Cockpit)
- `[x]` Audit Cybersecurity Toolchain (Nmap, Wireshark, SQLMap, Binwalk, etc.)
- `[x]` Audit Developer Toolchain (GCC, Python, Node, Cargo, Go, Neovim)
- `[x]` Verify user configuration files & skel templates
- `[x]` Automatic self-healing repair mode (`ri-doctor fix`)
- `[x]` Diagnostic log reporting (`ri-doctor report`)

## Phase 8: Testing, CI/CD & Quality Assurance
- `[x]` Static validation test suite (`tests/run-all.sh`, 15 automated test scripts)
- `[x]` QEMU End-to-End Installation test (`tests/qemu/install-test.sh`)
- `[x]` Verification of independent boot with ISO detached
- `[x]` GitHub Actions CI workflows (`build.yml`, `tests.yml`, `iso.yml`)
- `[x]` Documentation: `ARCHITECTURE.md`, `BUILD.md`, `INSTALL.md`, `DESKTOPS.md`, `SECURITY.md`, `TESTING.md`, `TROUBLESHOOTING.md`, `RELEASE.md`

## Phase 9: Advanced Bare-Metal Tooling & Utilities
- `[x]` **Safe USB Flasher (`ri-flash`)**: Removable USB drive scanner with root safety locks and direct dd synchronization
- `[x]` **Cybersecurity Hub (`ri-cyber`)**: Security auditing, network recon, port scan, SSL audit, and web check CLI
- `[x]` **Developer Hub (`ri-dev`)**: Project generator (Python, Node, Rust, Go), container checker, and toolchain doctor
- `[x]` **Kali Linux Integrator (`ri-kali-setup`)**: Low-priority APT pinning (Priority 100) for 600+ Kali Rolling packages
- `[x]` Synchronize helper tools to Live/Installed chroot `/usr/local/bin/`

---

## Phase 10: Real Bare-Metal Media Flashing & Media Integrity
- `[x]` Automated checksum generator for SHA256 and SHA512 during build
- `[x]` Multi-partition USB layout support (Live ISO partition + persistent storage partition)
- `[x]` Safe USB Flashing utility with removable device locks (`ri-flash`)
- `[x]` Media integrity & live squashfs self-test validator (`ri-media-check`)
- `[ ]` Verification of USB boot media on Ventoy, Rufus (DD mode), and BalenaEtcher
- `[ ]` GPG signature validation on installation media (`RiOS.iso.sig`)

## Phase 11: Real Computer UEFI/BIOS Boot & Storage Architecture
- `[x]` Dual-mode boot compatibility: Legacy MBR BIOS (Syslinux) + UEFI 64-bit (GRUB-EFI)
- `[x]` Non-destructive internal NVMe, SATA SSD, and HDD enumeration
- `[ ]` Validation on diverse physical computer hardware (Intel Core Gen 6-14, AMD Ryzen 1000-7000)
- `[ ]` Validation on Apple Intel T2 and UEFI MacBooks
- `[ ]` Support for Secure Boot signed boot chain with Debian Shim loader

## Phase 12: GPU Driver Stacks & Hardware Graphics Acceleration
- `[x]` Open-source graphics drivers included: Mesa, Gallium, Intel Iris/Xe, AMD Radeon R600/RadeonSI
- `[x]` Vulkan runtime components (`mesa-vulkan-drivers`, `vulkan-tools`)
- `[x]` Automated proprietary NVIDIA driver installation helper (`ri-gpu-setup nvidia`)
- `[x]` Hybrid graphics PRIME render offload switching guide (`ri-gpu-setup prime`)
- `[x]` Hardware video decode acceleration validation (`ri-gpu-setup status`)

## Phase 13: Kernel Optimization & Low-Latency Tuning
- `[x]` Default distribution kernel: Debian 6.1 LTS Bookworm Linux Kernel
- `[ ]` High-performance kernel options: Zen kernel / Liquorix kernel package integration
- `[ ]` Sysctl performance profile optimizations for I/O throughput and responsiveness (`/etc/sysctl.d/99-rios-perf.conf`)
- `[ ]` Real-time audio privilege configuration (`limits.d/audio.conf` for RT priority)
- `[ ]` CPU governor automated switching (Performance on AC power, Powersave on Battery)

## Phase 14: Kali Linux Deep Security Ecosystem Integration
- `[x]` 76+ Core offensive security packages pre-installed
- `[x]` Kali Rolling repository integration script (`ri-kali-setup`)
- `[x]` Automated APT pinning configuration (preventing Debian core library collision)
- `[ ]` Pre-packaged Metasploit Framework automated installer recipe
- `[ ]` Wordlist bundle provisioner (RockYou, SecLists, DirBuster dictionaries in `/usr/share/wordlists`)
- `[ ]` Wireless monitor mode helper script (`ri-airmon`)

## Phase 15: Zero-Trust Hardening, AppArmor & Cryptographic Isolation
- `[x]` UFW stateful firewall and fail2ban pre-installed
- `[x]` Automated zero-trust hardening script (`ri-harden all`)
- `[x]` CVE vulnerability auditor & automated security patching engine (`ri-cve`)
- `[x]` CPU hardware vulnerability mitigations audit (Spectre, Meltdown, Retbleed)
- `[x]` Hardened kernel security sysctls applied (`/etc/sysctl.d/99-rios-security.conf`)
- `[x]` OpenSSH daemon root login restrictions and key enforcement
- `[ ]` Default AppArmor profiles enforced for network daemons (Nginx, SSH, Apache, BIND)
- `[ ]` Lynis automated compliance auditing scoring > 80/100 out of the box

## Phase 16: Containerization, Kubernetes & Microservices Infrastructure
- `[x]` Docker CE and Podman pre-configured
- `[x]` Container socket management and group permission configuration
- `[x]` OCI / Docker Container RootFS Image Builder (`build.sh container` / `scripts/build_container_image.sh`)
- `[ ]` Single-node Kubernetes starter (`k3s` automated deployment script `ri-k3s`)
- `[ ]` Container networking verification (CNI bridge, overlay, port forwards)
- `[ ]` Pre-installed compose tooling (`docker-compose-v2` / `podman-compose`)

## Phase 17: Resilient Enterprise Storage (Btrfs, ZFS, LVM RAID)
- `[x]` Standard EXT4 + FAT32 EFI partition layout
- `[ ]` Btrfs root filesystem with default subvolumes (`@`, `@home`, `@snapshots`, `@var_log`)
- `[ ]` LVM (Logical Volume Manager) automated partitioning option in `ri-installer`
- `[ ]` ZFS on Linux kernel modules support in `rios-server` profile
- `[ ]` Software RAID 1/5/10 creation options for enterprise server multi-drive arrays

## Phase 18: Full-Disk Encryption & Hardware TPM2 Automated Unlocking
- `[ ]` LUKS2 (Linux Unified Key Setup) volume encryption support in `ri-installer`
- `[ ]` Argon2id PBKDF cryptographic key derivation for root volume protection
- `[ ]` TPM2 (Trusted Platform Module) automated disk decryption integration (`systemd-cryptenroll`)
- `[ ]` FIDO2 USB hardware security key unlock support (YubiKey integration)
- `[ ]` Secure emergency recovery passphrase generation and export

## Phase 19: High-Availability Server Infrastructure & Clustering
- `[x]` Core server services: OpenSSH, Nginx, Docker, UFW, Wireguard, Cockpit
- `[ ]` High-availability virtual IP failover configuration (`keepalived`)
- `[ ]` Reverse proxy load-balancing starter templates for Nginx & HAProxy
- `[ ]` GlusterFS / Ceph distributed storage client packages
- `[ ]` Multi-node WireGuard mesh configuration wizard (`ri-vpn-mesh`)

## Phase 20: PXE, iPXE & Diskless Network Installation Architecture
- `[x]` TFTP & iPXE network bootloader configuration bundle (`boot.ipxe`, `pxelinux.cfg/default`)
- `[x]` Netboot & PXE Network Asset Builder (`build.sh netboot` / `scripts/build_netboot.sh`)
- `[x]` Local HTTP network boot asset streaming server launcher (`build/netboot/serve.sh`)
- `[ ]` Diskless workstation mode (RiOS running entirely in RAM over network)
- `[ ]` Lab deployment guide for computer clubs, schools, and security CTF ranges

## Phase 21: Automated Unattended Deployment (Preseed & Kickstart)
- `[ ]` Debian preseed configuration file (`preseed.cfg`) for zero-touch physical installation
- `[ ]` Unattended ISO boot option (`Install RiOS (Automated)` in bootloader menu)
- `[ ]` Post-installation provisioner hook for automated Ansible playbook execution
- `[ ]` Cloud-init configuration support for bare-metal cloud deployments
- `[ ]` Verification of automated deployment on virtual and bare-metal targets

## Phase 22: Self-Hosted APT Mirror & Custom Package Repository
- `[ ]` Debian package repository creation script (`reprepro` / `aptly`)
- `[ ]` Dedicated RiOS custom packages `.deb` build workflow (GPG-signed repository)
- `[ ]` Custom package signing key generation and keyring package (`rios-archive-keyring`)
- `[ ]` Automated package repository mirroring script for offline military/secure air-gapped networks
- `[ ]` GitHub Releases or CDN repository hosting setup

## Phase 23: Atomic Upgrades, OTA Updates & System Rollbacks
- `[x]` System update command line tool (`ri-update`)
- `[ ]` Pre-update automatic snapshot creation via Snapper or Timeshift
- `[ ]` GRUB boot menu snapshot integration (boot into previous working state on update failure)
- `[ ]` Background unattended security updates configuration (`unattended-upgrades`)
- `[ ]` System rollback CLI tool (`ri-rollback`)

## Phase 24: Wayland High-DPI Multi-Monitor & Tiling Window Management
- `[x]` Functional Hyprland configuration with animations, keybinds, and rules
- `[x]` Waybar status panel and Sway fallback compositor
- `[ ]` High-DPI fractional scaling auto-detection (4K / 2K laptop display support)
- `[ ]` Multi-monitor layout manager script (`ri-monitors`) with hotplug support
- `[ ]` Wayland desktop screen recording and presentation tools (`wl-screenrec`, `wf-recorder`)

## Phase 25: Low-Latency PipeWire Audio/Video Multimedia Pipeline
- `[x]` ALSA and PipeWire sound architecture with pavucontrol mixer
- `[ ]` Pro-audio low-latency PipeWire buffer configuration (`quantum = 64/48000`)
- `[ ]` Bluetooth audio codec support (LDAC, aptX, AAC)
- `[ ]` OBS Studio and screen sharing through `xdg-desktop-portal-wlr` / `xdg-desktop-portal-gtk`
- `[ ]` System sound effects theme for notifications and desktop events

## Phase 26: Internationalization & Regional Localization (Khmer & CJK)
- `[x]` Khmer OS fonts and Noto Color Emoji included
- `[x]` Default timezone: Asia/Phnom_Penh in network configuration
- `[ ]` IBus / Fcitx5 Khmer input method engine pre-configured
- `[ ]` Multilingual support for ASEAN languages (Khmer, Thai, Vietnamese, Indonesian)
- `[ ]` CJK font fallbacks (`fonts-noto-cjk`) for Asian character rendering

## Phase 27: Disaster Recovery, System Snapshotting & Backups
- `[ ]` Timeshift / Snapper pre-installed with hourly/boot snapshot rules
- `[ ]` Emergency rescue shell boot entry in GRUB (`RiOS Recovery Mode`)
- `[ ]` Full-system offline backup utility (`ri-backup`) to external storage or NAS
- `[x]` Live USB Chroot Rescue Tool (`ri-chroot`) to easily repair broken systems
- `[ ]` Automated MBR/GPT partition table backup on installation

## Phase 28: Performance Telemetry, Observability & Health Metrics
- `[x]` Real-time host metrics CLI (`ri-server info`)
- `[ ]` Prometheus node-exporter service configuration (`ri-server enable prometheus`)
- `[ ]` Cockpit Performance & Storage Monitoring module integration
- `[ ]` GPU utilization monitor CLI (`nvtop` / `radeontop`)
- `[ ]` Storage health telemetry monitoring daemon (`smartmontools` + `smartctl`)

## Phase 29: Cloud Images, Virtual Appliances & Hypervisor Templates
- `[x]` Standard QCOW2 cloud image build target (`build.sh cloud` / `scripts/build_cloud_image.sh`)
- `[ ]` Vagrant box recipe for local developer provisioning
- `[ ]` Proxmox VE / KVM LXC & VM template creation script
- `[ ]` OVA / OVF virtual appliance export for VMware ESXi and VirtualBox
- `[ ]` AWS EC2 and Google Cloud Platform custom AMI image generation scripts

## Phase 30: Supply Chain Security, SBOM & Cryptographic Image Signing
- `[ ]` Software Bill of Materials (SBOM) generation during build (`syft` / `spdx` JSON)
- `[ ]` Sigstore Cosign cryptographic signing of release ISO and container builder
- `[ ]` Reproducible build verification (ensuring identical bit-for-bit hashes)
- `[ ]` Automated vulnerability scanning of rootfs packages with Trivy
- `[ ]` Secure boot MOK (Machine Owner Key) enrollment documentation

## Phase 31: Documentation Ecosystem, Knowledge Base & Developer Portals
- `[x]` Core docs: `ARCHITECTURE.md`, `BUILD.md`, `INSTALL.md`, `SECURITY.md`, `DESKTOPS.md`, `TESTING.md`, `RELEASE.md`
- `[ ]` Comprehensive Man Pages (`man ri-installer`, `man ri-server`, `man ri-cyber`, `man ri-doctor`)
- `[ ]` Web-based Documentation Portal using MkDocs / VitePress
- `[ ]` Step-by-step Hardware Installation Walkthrough with real screenshots
- `[ ]` Cybersecurity Lab CTF walk-through guide using RiOS

## Phase 32: Release Candidate Quality Gates & GA Release Engineering
- `[x]` Release candidate build validation: `rios-live-amd64.hybrid.iso` (4.14 GB, PASS)
- `[x]` QEMU End-to-End installation test with ISO detached: PASS
- `[ ]` Final QA checklist validation across 10 distinct physical hardware testbeds
- `[ ]` Official Git Release Tag: `v1.0.0-GA`
- `[ ]` Public ISO release hosting with global mirrors and Torrent / Magnet links
- `[ ]` Official Announcement and Release Notes distribution

## Phase 33: Enterprise LTS Lifecycle, Security Advisory & Governance
- `[ ]` 5-Year Long Term Support (LTS) maintenance roadmap aligned with Debian 12
- `[ ]` RiOS Security Advisory (RSA) publishing channel for critical CVE alerts
- `[ ]` Automated CVE patch distribution mechanism via dedicated apt security channels
- `[ ]` Community issue triage guidelines and code of conduct
- `[ ]` Enterprise contribution governance model and bug bounty program

## Phase 34: Systemd Service Hardening & Host Environment Style Parity
- `[x]` Audit and harden systemd units (`ri-first-boot.service`, `rios-firstboot.service`, `rios-health.service`)
- `[x]` Implement lightweight background health monitoring daemon (`rios-health-daemon`)
- `[x]` Deploy systemd default preset policies (`/usr/lib/systemd/system-preset/99-rios.preset`)
- `[x]` Deploy chroot live-build preset enablement hook (`0130-systemd-enable.hook.chroot`)
- `[x]` Enforce automated systemd unit testing (`tests/test-systemd.sh`) in test pipeline
- `[x]` Replicate host machine Starship cross-shell prompt with two-line layout and folder icons
- `[x]` Replicate host Kitty terminal configuration, acrylic blur opacity, and dark theme palette
- `[x]` Deploy modern Waybar floating island design with rounded pill modules and hardware sensors
- `[x]` Align Sway window manager borders, gaps, font, and palette with host desktop
- `[x]` Deploy host Fastfetch telemetry layout with RiOS branding to `etc/skel/.config/fastfetch/`
- `[x]` Deploy complete shell skeleton dotfiles (`.bashrc`, `.zshrc`, `.profile`) into `etc/skel/`

## Phase 35: 2026 Full System Master Update & 4-Edition Architecture (All, Server, Cyber, Dev)
- `[x]` Upgrade OS release identity to RiOS 2026 Full System (`/etc/os-release`, `VERSION="2026.1 LTS"`)
- `[x]` Update system ASCII banners in `/etc/issue`, `/etc/issue.net`, `/etc/motd`, and bootloaders
- `[x]` Upgrade all core system utilities, self-healing diagnostics, and session managers to 2026
- `[x]` Establish and orchestrate all 4 system editions: All/Full, Server, Cyber, and Dev
- `[x]` Implement multi-edition batch builder & validator `./build.sh all4`
- `[x]` Implement automated 4-edition profile test suite (`tests/test-editions.sh`)
- `[x]` Verify 100% test pass rate across all 17 automated static test suites


