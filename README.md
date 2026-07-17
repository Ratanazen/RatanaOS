# RatanaOS

RatanaOS is a Debian Stable-compatible desktop distribution. Phase 1 provides a
reproducible, minimal KDE Plasma live ISO with Debian Installer integration and
independent RatanaOS identity files.

## Phase 1 quick start

Build on an amd64 Debian Stable machine or container with at least 20 GiB free:

```bash
sudo apt update
sudo apt install live-build debootstrap squashfs-tools xorriso isolinux \
  syslinux-common grub-pc-bin grub-efi-amd64-bin mtools dosfstools
make test
sudo make build
```

The ISO is written to `build/artifacts/ratanaos-<version>-amd64.iso`. See
[`docs/phase-1.md`](docs/phase-1.md) for architecture, verification, and VM
boot instructions.

## Repository layout

- `build/`: generated live-build state and final artifacts
- `packages/`: Debian package sources and package lists
- `branding/`: operating-system identity and artwork sources
- `themes/`: Plasma, SDDM, Plymouth, GRUB, icon, and cursor themes
- `installer/`: installer configuration and future custom frontend
- `apps/`: RatanaOS desktop applications
- `docs/`: architecture, contributor, and release documentation
- `scripts/`: repeatable build and maintenance commands
- `tests/`: static and ISO smoke tests

RatanaOS artwork and identity must be original and must not imply endorsement by
Debian. Debian is a trademark of Software in the Public Interest, Inc.

