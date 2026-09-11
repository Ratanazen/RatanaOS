# RatanaOS Troubleshooting & Recovery Guide

## 1. System Self-Diagnostics with `ratana-doctor`

RatanaOS includes an automated diagnostic and self-repair tool:

```bash
# 1. Audit system health:
ratana-doctor check

# 2. Automatically repair missing configurations and permissions:
ratana-doctor fix

# 3. Generate and export a full diagnostic report:
ratana-doctor report
```

---

## 2. Boot Issues & Disk Recovery

### Issue: "Initramfs cannot find boot medium"
- **Cause**: Happens when `live-boot` was not removed from the target disk.
- **Solution**: Boot the Live USB, open a terminal, and run:
  ```bash
  sudo mount /dev/sdX2 /mnt
  sudo mount /dev/sdX1 /mnt/boot/efi # if UEFI
  sudo chroot /mnt apt-get purge -y live-boot live-config
  sudo chroot /mnt update-initramfs -u -k all
  sudo chroot /mnt update-grub
  ```

### Issue: NVIDIA Black Screen / Modesetting
- Add `nvidia-drm.modeset=1` to `GRUB_CMDLINE_LINUX_DEFAULT` in `/etc/default/grub` and run `sudo update-grub`.
