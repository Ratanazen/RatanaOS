# RatanaOS build environment

RatanaOS Phase 1 uses Debian `live-build` to produce an amd64 hybrid live and
installer ISO. Run builds on Debian Stable (or a compatible Debian container)
with root privileges and at least 20 GiB of free disk space.

## Required packages

Install the build and test dependencies:

```bash
sudo apt update
sudo apt install git live-build debootstrap squashfs-tools xorriso \
  isolinux syslinux-common grub-pc-bin grub-efi-amd64-bin mtools dosfstools \
  qemu-system-x86 qemu-utils make ca-certificates
```

The principal commands supplied by these packages are `git`, `lb`,
`debootstrap`, `mksquashfs`, `xorriso`, the GRUB utilities, and
`qemu-system-x86_64`.

## Build

From the repository root, validate the source tree and build the ISO:

```bash
./build/test.sh
sudo ./build/build.sh
```

The ISO and its SHA-256 checksum are written to `build/artifacts/`. To remove
generated live-build state while retaining release artifacts, run:

```bash
sudo ./build/clean.sh
```

The equivalent Make targets are `make test`, `sudo make build`, and
`sudo make clean`.

## Test

Run static validation without an ISO:

```bash
./build/test.sh
```

Validate the ISO boot catalog and start an interactive QEMU boot test:

```bash
./build/test.sh build/artifacts/ratanaos-<version>-amd64.iso
```

The QEMU test defaults to 4 GiB RAM and two virtual CPUs. Override these with
`QEMU_MEMORY` and `QEMU_CPUS`. Hardware acceleration requires KVM; if KVM is
unavailable, remove `-enable-kvm` from a local copy of the QEMU command shown in
`test.sh` or run the ISO directly with QEMU using software emulation.

During the interactive test, verify that the firmware reaches the RatanaOS boot
menu, the live session starts, the desktop loads, and the installer launches.

## Troubleshooting

- **Missing command:** install the package listed above that supplies it, then
  rerun `./build/test.sh` before building.
- **Build requires root:** run `sudo ./build/build.sh`; live-build must mount
  filesystems while constructing the image.
- **Package download or debootstrap failure:** confirm DNS, network access,
  Debian mirror availability, system time, and available disk space. Set
  `MIRROR` to a reachable Debian mirror if necessary.
- **Stale live-build state:** run `sudo ./build/clean.sh`, then rebuild. This
  retains files already copied into `build/artifacts/`.
- **QEMU cannot use KVM:** ensure virtualization is enabled and the user can
  access `/dev/kvm`, or use QEMU software emulation without `-enable-kvm`.
- **ISO fails boot-catalog validation:** inspect the live-build log and confirm
  that GRUB, isolinux, syslinux, and xorriso packages are installed.
- **Out of disk space:** provide at least 20 GiB free and clean generated build
  state before retrying.
