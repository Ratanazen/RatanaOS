# RatanaOS v5.0 "Phoenix" — Migration Guide

This guide helps users running RatanaOS v4.x upgrade to v5.0 Phoenix.

## What's New in v5.0

See [CHANGELOG.md](../CHANGELOG.md) for the full list. Major highlights:

- **Atomic updates** — if you enable Btrfs on a fresh install, you can roll back any update in seconds.
- **ARM64 support** — Raspberry Pi 4/5 and other ARM64 single-board computers now have a dedicated edition.
- **Two new editions** — RatanaOS Server (headless, ~800 MB) and RatanaOS ARM.
- **New apps** — Ratana Firewall, Ratana Recovery, and the full Ratana Assistant.

---

## Upgrade Path

### Option A: Fresh Install (Recommended)

The cleanest route to v5.0 is a fresh installation using the new ISO.

1. Back up your data using Ratana Backup or `rsync`.
2. Download the v5.0 Phoenix ISO matching your edition.
3. Boot the Live USB and run the installer.
4. During the Disk step, select "Automatic Partitioning" with **Btrfs** enabled for atomic updates.

### Option B: In-place Upgrade (x86_64 only)

An in-place upgrade is possible if you are already on RatanaOS v4.x running Debian Bookworm.

```bash
# Add the v5.0 repository
sudo apt update
sudo apt dist-upgrade -y

# Install new v5.0 packages
sudo apt install ratana-firewall ratana-recovery flatpak snapd
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

# Enable Btrfs snapshots (only if /home is on Btrfs)
sudo apt install timeshift
sudo timeshift --create --comments "Before v5.0 upgrade"
```

---

## Breaking Changes

| Area | Change |
|---|---|
| ISO names | Renamed to `RatanaOS-Lite.iso`, `RatanaOS-Standard.iso`, etc. |
| Installer | 11 steps (was 9) — Edition and Update Strategy are now separate pages |
| Encryption | LUKS1 → LUKS2 on fresh installs |
| Checksums | Both SHA256 *and* SHA512 are now generated |

---

## Notes

- ARM editions cannot be upgraded in-place from x86_64. Use a fresh install on your SBC.
- Server edition has no graphical installer — use the unattended install mode or a preseed file.
