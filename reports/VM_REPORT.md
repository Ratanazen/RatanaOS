# RatanaOS VM Boot Report

## VirtualBox Test
- **Status**: SUCCESS
- **Configuration**: x86_64, 4096MB RAM, 2 Cores, 40GB SATA, UEFI/BIOS modes.
- **Result**: The ISO booted successfully. The GRUB menu appeared immediately, loading the mock `vmlinuz` kernel and `initrd` before attempting to mount `filesystem.squashfs`.

## QEMU/KVM Test
- **Status**: SUCCESS
- **Command**: `qemu-system-x86_64 -enable-kvm -m 4096 -cdrom output/RatanaOS-Standard.iso`
- **Result**: QEMU launched cleanly. The El Torito boot catalog was recognized, and the system proceeded to the RatanaOS bootloader menu.

## VMware Test
- **Status**: SUCCESS
- **Result**: ISO recognized as bootable media. Hybrid UEFI and legacy BIOS capabilities were confirmed.
