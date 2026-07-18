# RatanaOS Changelog

## v18.0.0 "Recovery & Reliability"
*A robust suite of rescue tools for mission-critical systems.*

### Features
- **Recovery Mode**: Dedicated GRUB boot entry booting directly into `ratana-recovery`.
- **Ratana Recovery Overhaul**: Added new tools including System Log Viewer, Emergency Shell, Boot Repair, and Hardware/Boot Diagnostics.
- **Crash Reporting**: Export segmentation faults and core dumps from `/var/crash` to external media.
- **Documentation**: New `docs/RECOVERY.md` detailing disaster recovery workflows.

---

## v16.0.0 "Modular UI & Assets"
*Zero-code UI personalization engine.*

### Features
- **Modular Assets**: Introduced unified `/assets/` directory structure for icons, wallpapers, avatars, and cursors.
- **Configuration Engine**: Built `ratanaos-config.yaml` to orchestrate default configurations.
- **Redesigned Installer**: 12-page Qt6 wizard scanning dynamic assets directly from the filesystem.

---

## v5.0.0 "Phoenix" — In Development

### New Editions
- **RatanaOS Server** — headless, ~800 MB ISO, Docker/Podman, automated security patching, optional web-based control panel
- **RatanaOS ARM** — Raspberry Pi 4/5, GPIO tooling, hardware-optimized kernel, power management profile

### New Features
- ARM64 cross-compilation build target
- Atomic update + Btrfs snapshot rollback support
- Flatpak and Snap support out of the box
- Opt-in anonymized telemetry (disabled by default)
- Accessibility suite: screen reader, high-contrast themes, magnifier
- Cloud backup/sync integration (user-controlled)
- LUKS2 full-disk encryption in installer
- Unattended/scripted install mode

### New Applications
- **Ratana Assistant** — full AI copilot with system-wide context
- **Ratana Firewall** — GUI for UFW/nftables
- **Ratana Recovery** — bootable rescue toolkit

### Installer Changes
- Added step: Update Strategy (APT vs. Atomic/Rollback)
- LUKS2 support, UEFI Secure Boot enhancements

### Build System
- SHA256 + SHA512 dual checksums
- Software Bill of Materials (SBOM) generation

### New Documentation
- `SECURITY.md`, `ACCESSIBILITY.md`, `MIGRATION_GUIDE.md`

---

## v1.0.0 "Cyber Edition"
*Initial release — July 2026.*

### Features
- Qt6 `RatanaUI` widget system
- Qt6 graphical installer (9-step wizard)
- 4 build profiles: `ratana-cyber`, `ratana-developer`, `ratana-lite`, `ratana-workstation`
- ISO builder with SHA256 checksums, build reports
- AppArmor, UFW, Auditd security integration
- CI/CD pipeline

### Applications
- Ratana Terminal
- Ratana Settings
- Ratana Software Center
- Ratana Update Manager
- Ratana System Monitor
- Ratana File Manager
- Ratana Welcome
- RatanaAI Assistant
