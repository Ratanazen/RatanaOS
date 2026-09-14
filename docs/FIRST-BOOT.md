# RiOS First-Boot Architecture (Stage B)

## Overview
RiOS distinguishes between **Stage A (OS Installation)** and **Stage B (First-Boot Provisioning & Rice)**. When your computer boots from the internal disk for the very first time, RiOS automatically runs a self-disabling hardware configuration sequence.

---

## 1. Systemd Service: `ri-first-boot.service`

- **Location**: `/etc/systemd/system/ri-first-boot.service`
- **Execution**: Runs once as a `oneshot` service before `display-manager.service`.
- **Tasks**:
  1. **Hardware Detection**: Scans GPU vendor (`lspci`) and memory capacity (`free -m`).
  2. **Performance Profile**: Automatically selects:
     - `low`: For systems with < 4GB RAM (disables heavy blur & shadows).
     - `balanced`: Default standard profile.
     - `beautiful`: For systems with > 15GB RAM and dedicated GPUs.
  3. **Theme Application**: Sets default theme to `ri-dark` in `/etc/rios/theme.conf`.
  4. **Font Cache**: Re-indexes system typography via `fc-cache -f`.
  5. **Permissions**: Audits `/home/*` ensuring non-root ownership.
  6. **Self-Disable**: Touches `/var/lib/rios/.first-boot-done` and executes `systemctl disable ri-first-boot.service`.

---

## 2. User-Level First Login: `ri-first-run`

When the user logs in for the first time:
- The script checks for `~/.config/ri/.first-run-complete`.
- If missing, it safely backs up any existing configurations to `~/.config/ri/backups/TIMESTAMP/`.
- Initializes Waybar, default wallpaper (`rios-sequoia-dark.png`), SwayNC, and Kitty.
- Touches `~/.config/ri/.first-run-complete` to ensure idempotency.
