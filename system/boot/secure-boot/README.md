# RatanaOS Secure Boot Support

RatanaOS targets UEFI Secure Boot using the standard Debian-compatible shim flow.

## Boot Chain

1. Firmware verifies `shimx64.efi`.
2. `shim` verifies GRUB with embedded vendor certificate trust.
3. GRUB verifies the signed kernel and initramfs policy.
4. The kernel enforces lockdown when Secure Boot is active.

## Required Packages

- `shim-signed`
- `mokutil`
- `sbsigntool`
- `efibootmgr`

## Requirements

- Sign first-party EFI binaries and kernels before image publication.
- Store signing material outside the build workspace and CI logs.
- Fail image release when unsigned EFI artifacts are detected.
- Enroll Machine Owner Keys only for developer and recovery workflows.

## Release Gate

- Secure Boot must boot cleanly on reference UEFI hardware before alpha.
- Kernel command line must include AppArmor and lockdown settings.

