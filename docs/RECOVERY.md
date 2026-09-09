# 🔧 RatanaOS Backup & Recovery Guide (Phase 15)

## System Snapshots with Timeshift

RatanaOS includes **Timeshift** for filesystem snapshot and restore
functionality, pre-installed in the desktop variant.

### Taking a Snapshot
1. Launch Timeshift from the application menu or run \`sudo timeshift-gtk\`.
2. Select snapshot type:
   - **RSYNC**: Works on all filesystem types. Recommended for most users.
   - **BTRFS**: Native snapshots if using Btrfs filesystem (requires Btrfs
     root partition — configured during Calamares install).
3. Click "Create" to take an immediate snapshot.

### Restoring from a Snapshot
1. Launch Timeshift (\`sudo timeshift-gtk\`).
2. Select the snapshot to restore from the list.
3. Click "Restore" and confirm.
4. Reboot when prompted.

### Automated Snapshot Schedule
Timeshift is installed but **not pre-configured with a schedule** by
default. Users configure their preferred schedule (hourly/daily/weekly)
via the Timeshift GUI Settings tab. This matches standard Debian behavior
— no assumptions are made about the user's backup preferences.

---

## GRUB Recovery Mode

### Accessing Recovery Mode
1. During boot, hold **Shift** (BIOS) or press **Esc** (UEFI) to display
   the GRUB boot menu.
2. Select the kernel entry ending in **(recovery mode)**.
3. The system boots into a root shell with networking disabled.

### Recovery Mode Capabilities
- Fix broken packages: \`dpkg --configure -a && apt --fix-broken install\`
- Reset user password: \`passwd <username>\`
- Check filesystems: \`fsck /dev/sdXN\`
- Reinstall GRUB: \`grub-install /dev/sdX && update-grub\`
- Restore Timeshift snapshot from CLI: \`timeshift --restore\`

### Verification
RatanaOS's custom GRUB theme does NOT suppress recovery mode entries.
The \`0300-grub-recovery.hook.chroot\` hook explicitly ensures
\`GRUB_DISABLE_RECOVERY\` is never set to "true", guaranteeing recovery
entries always appear in the GRUB menu alongside normal boot entries.

---

## Emergency: Desktop Won't Start

If the XFCE desktop fails to start after an update or configuration change:

1. Boot into recovery mode (see above).
2. Reset XFCE configuration:
   \`\`\`bash
   rm -rf /home/<username>/.config/xfce4
   rm -rf /home/<username>/.config/plank
   \`\`\`
3. Reboot normally — XFCE will regenerate defaults from \`/etc/skel/\`.
