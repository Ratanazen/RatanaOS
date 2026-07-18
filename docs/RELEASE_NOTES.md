# RatanaOS v2.0 Release Notes

Welcome to RatanaOS v2.0! This release marks a total transformation of the platform, bringing an original user experience on top of a highly stable Debian foundation.

## Key Features

- **Custom Desktop Environment**: Powered by Qt6, the new desktop architecture supports multiple profiles (GNOME, KDE, XFCE) while maintaining a unique, modular `RatanaUI` global aesthetic with native dark mode support.
- **Ratana Terminal**: A GPU-accelerated workspace terminal with split-pane support and deep SSH profile integrations.
- **Ratana File Manager**: A streamlined, dual-pane explorer with a modernized "Places" sidebar and rapid breadcrumb navigation.
- **RatanaAI Assistant**: A native, on-device AI copilot capable of explaining complex shell commands, troubleshooting system logs, and providing package recommendations.
- **Unified Software Center**: Complete graphical integration with `APT` and `Flatpak`, featuring live dynamic search and one-click installs.
- **Resilient Update Manager**: A background daemon coordinates system upgrades with support for Beta channels, audit tracking, and Btrfs snapshot rollbacks.
- **Hardened Security**: 
  - Shipped with default AppArmor Mandatory Access Control profiles.
  - Active Uncomplicated Firewall (UFW) with a default-deny ingress policy.
  - Full UEFI Secure Boot compliance (`shim-signed`).

## Build & Release Architecture

- Automated CI/CD pipelines validate system integrity prior to ISO generation.
- The ISO builder seamlessly creates hybrid UEFI/BIOS bootable images.
- System configurations and package manifests are version-controlled via Debian Preseed.

*Thank you to all contributors who made RatanaOS v2.0 a reality!*
