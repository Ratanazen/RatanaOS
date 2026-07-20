# RatanaOS Supported Hardware Matrix

RatanaOS maintains strict driver stability and broad hardware compatibility standards across modern PC hardware.

---

## Hardware Compatibility Matrix

| Hardware Category | Supported Chipsets & Vendors | Primary Kernel Modules / Drivers | Status |
|---|---|---|---|
| **CPU (x86_64)** | Intel Core (2nd–14th Gen), Xeon, AMD Ryzen, EPYC, Athlon | Linux Kernel + `intel-microcode` / `amd64-microcode` | ✅ Verified |
| **GPU (AMD)** | Radeon HD 7000+, RX 400–7000 Series, Ryzen Vega/RDNA APUs | `amdgpu` + Mesa RADV Vulkan + VA-API | ✅ Verified |
| **GPU (Intel)** | HD 2000–6000, Iris Xe, Arc A-Series GPUs | `i915` / `xe` + Mesa Iris + VA-API | ✅ Verified |
| **GPU (NVIDIA)** | GeForce GTX 600–RTX 4000 Series, Quadro | `nouveau` (Default Open-Source) / `nvidia-driver` (Optional) | ✅ Verified |
| **Storage (NVMe)** | PCIe NVMe 1.3/1.4/2.0 SSDs (Samsung, WD, Crucial, Kingston, Kioxia) | `nvme` kernel module | ✅ Verified |
| **Storage (SATA/HDD)** | AHCI SATA III SSDs, HDDs, Optical Drives | `ahci`, `libata`, `sd_mod` | ✅ Verified |
| **Storage (eMMC/USB)**| Embedded Flash eMMC, USB 2.0/3.x Flash Drives, External SSDs | `sdhci`, `sdhci-pci`, `usb-storage`, `uas` | ✅ Verified |
| **Storage Encryption**| Full Disk & Partition Encryption (LUKS2) | `dm-crypt`, `cryptsetup` | ✅ Verified |
| **Logical Storage** | Logical Volume Manager (LVM2), Software RAID (mdadm) | `dm-mod`, `mdadm` | ✅ Verified |
| **Filesystems** | ext4, Btrfs (with snapshot support), XFS, FAT32/exFAT | `ext4`, `btrfs`, `xfs`, `vfat` | ✅ Verified |
| **Wi-Fi Cards** | Intel AX200/AX210/BE200, Realtek RTL8821/8822, Broadcom, Atheros | `firmware-iwlwifi`, `firmware-realtek`, `firmware-brcm80211` | ✅ Verified |
| **Ethernet** | Intel I219/I225/I226, Realtek RTL8111/8125, Marvell, Broadcom | `e1000e`, `igc`, `r8169`, `tg3` | ✅ Verified |
| **Bluetooth** | Integrated Intel/Realtek/Broadcom Bluetooth 4.x/5.x Adapters | `bluez`, `btusb` | ✅ Verified |
| **Audio Controllers**| Intel HD Audio, AMD HD Audio, Realtek ALC Codecs, USB Audio | PipeWire + WirePlumber + `snd-hda-intel` | ✅ Verified |
| **Laptop Mics** | Sound Open Firmware (SOF) Digital Microphone Arrays | `firmware-sof-signed` | ✅ Verified |
| **Touchpads** | Synaptics, ELAN, ALPS, Apple multi-touch touchpads | `libinput` | ✅ Verified |
| **Touchscreens** | e-Galax, Atmel, I2C/USB Touchscreen Digitizers | `libinput`, `hid-multitouch` | ✅ Verified |
| **Game Controllers** | Xbox 360/One/Series, PlayStation DualShock 4 / DualSense, Generic USB | `xpad`, `hid-playstation`, `joydev`, `evdev` | ✅ Verified |
| **Printers** | HP, Canon, Epson, Brother (USB & Network Printers) | CUPS + `hplip` + `printer-driver-all` | ✅ Verified |
| **Webcams** | USB Video Class (UVC) Integrated & External Webcams | `uvcvideo` (`/dev/video*`) | ✅ Verified |
| **Power & Laptop** | Battery Management, ACPI Power Profiles, Suspend (S3/S0ix) | `power-profiles-daemon`, `TLP`, `acpi` | ✅ Verified |

---

## Boot & Firmware Mode Matrix

| Feature | Legacy BIOS | UEFI Mode | Secure Boot | Status |
|---|---|---|---|---|
| **Live USB Boot** | Supported | Supported | Supported | ✅ Verified |
| **GPT Partitioning** | Supported (via BIOS boot part) | Native | Native | ✅ Verified |
| **MBR Partitioning** | Native | Supported | Supported | ✅ Verified |
| **NVMe Root Boot** | Unsupported (MBR limits) | Native | Native | ✅ Verified |
