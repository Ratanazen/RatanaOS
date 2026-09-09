# 🔀 Track A — RatanaOS & Debian Dual Boot Guide

RatanaOS supports dual-booting with **Debian GNU/Linux** via GRUB 2 bootloader configurations.

---

## 📌 Dual Boot Modes

RatanaOS provides two dual-boot operating modes:

### Mode 1: Dual-Boot Existing Debian Partition
If Debian is installed on another partition on your disk (e.g., `/dev/sda2` or GPT partition 2):
1. Open `iso/boot/grub/grub.cfg`.
2. Locate the `Debian GNU/Linux (Dual Boot Partition)` entry.
3. Update `set root='(hd0,gpt2)'` and `root=/dev/sda2` to match your host disk's partition structure.

### Mode 2: Self-Contained Hybrid ISO with Debian Installer
You can embed a Debian installation ISO (such as `debian-12.0.0-amd64-netinst.iso`) directly into the RatanaOS bootable ISO image:

```bash
make iso-with-debian DEBIAN_ISO=/path/to/debian-netinst.iso
```

When built with `DEBIAN_ISO`, GRUB will automatically present a fourth option:
`Install / Boot Debian GNU/Linux (Embedded ISO)`.

---

## ⚠️ Partition Auto-Detection Limitations

RatanaOS runs a custom freestanding 64-bit kernel. It does **not** auto-probe host Linux GUID partition tables at boot time. You must manually configure GRUB partition identifiers (`(hd0,gptX)` or `/dev/sdaX`) in `iso/boot/grub/grub.cfg` for host partition dual-booting.

---

## 🧪 Verification in QEMU

To test the dual-boot menu in QEMU:
```bash
make iso
qemu-system-x86_64 -cdrom build/ratanaos.iso -m 512M
```
Select `RatanaOS 64-bit` or `Debian GNU/Linux` from the GRUB menu.
