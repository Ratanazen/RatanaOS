# Phase 1: minimal bootable ISO

## Architecture

`live-build` bootstraps Debian Stable, installs the package list, overlays files
from `branding/`, runs image hooks, creates a SquashFS live root, and produces a
hybrid BIOS/UEFI ISO. Debian Installer is embedded in `live` mode so an installed
system is copied from the tested live filesystem.

KDE Plasma is selected now because RatanaOS targets a conventional, deeply
customizable desktop. Phase 1 deliberately uses Debian's installer UI: writing a
partitioner is high-risk and does not help validate the base image. A branded
installer frontend and curated partitioning policy belong in a later phase.

The build configuration is generated rather than committed wholesale. This
keeps the source reviewable, makes suite/mirror changes explicit, and avoids
checking in live-build's generated state. `stable` tracks the current Debian
Stable release; production releases should pin a Debian codename and snapshot
mirror after a release candidate is cut.

## Build environment

Use Debian Stable amd64, root privileges, 4 GiB RAM, and at least 20 GiB free
disk. The host architecture must match `ARCH` unless binfmt/QEMU support is
configured. Install the dependencies shown in the project README, then run:

```bash
make test
sudo make build
```

Optional inputs are environment variables:

```bash
sudo SUITE=stable MIRROR=https://deb.debian.org/debian ARCH=amd64 make build
```

Build output and its SHA-256 checksum appear under `build/artifacts/`.

## Test and boot

Perform structural validation:

```bash
make test
./tests/smoke-iso.sh build/artifacts/ratanaos-*.iso
```

Then boot with QEMU (UEFI testing additionally requires OVMF):

```bash
qemu-system-x86_64 -enable-kvm -m 4096 -smp 2 \
  -cdrom build/artifacts/ratanaos-0.1.0-dev-amd64.iso -boot d
```

Verify the live session reaches Plasma, networking works, `/etc/os-release`
identifies RatanaOS, and the installer launches. Test installation only in a
disposable VM disk. Never point installer tests at a host disk.

## Reproducibility boundary

The process is repeatable but Phase 1 is not yet bit-for-bit reproducible because
`stable` and the Debian mirrors move as updates are published. Before the first
signed release, pin a codename, use `snapshot.debian.org`, set `SOURCE_DATE_EPOCH`,
record package manifests, and compare two clean build outputs.

## Next phases

1. Original visual identity, GRUB/Plymouth/SDDM themes, and Plasma defaults.
2. Installer branding, OEM-safe partition profiles, and automated VM installs.
3. Signed RatanaOS APT repository and metapackages.
4. Welcome, update, software, driver, backup, and settings applications.
5. Secure Boot signing, hardware qualification, accessibility, localization,
   release signing, rollback, and recovery media.

