# RatanaOS First-Boot Architecture (Stage B)

## Overview
RatanaOS distinguishes between **Stage A (OS Installation)** and **Stage B (First-Boot Provisioning & Rice)**. When your computer boots from the internal disk for the very first time, RatanaOS automatically runs a self-disabling hardware configuration sequence.

---

## 1. Systemd Service: `ratana-first-boot.service`

- **Location**: `/etc/systemd/system/ratana-first-boot.service`
- **Execution**: Runs once as a `oneshot` service before `display-manager.service`.
- **Tasks**:
  1. **Hardware Detection**: Scans GPU vendor (`lspci`) and memory capacity (`free -m`).
  2. **Performance Profile**: Automatically selects:
     - `low`: For systems with < 4GB RAM (disables heavy blur & shadows).
     - `balanced`: Default standard profile.
     - `beautiful`: For systems with > 15GB RAM and dedicated GPUs.
  3. **Theme Application**: Sets default theme to `ratana-dark` in `/etc/ratanaos/theme.conf`.
  4. **Font Cache**: Re-indexes system typography via `fc-cache -f`.
  5. **Permissions**: Audits `/home/*` ensuring non-root ownership.
  6. **Self-Disable**: Touches `/var/lib/ratanaos/.first-boot-done` and executes `systemctl disable ratana-first-boot.service`.

---

## 2. User-Level First Login: `ratana-first-run`

When the user logs in for the first time:
- The script checks for `~/.config/ratana/.first-run-complete`.
- If missing, it safely backs up any existing configurations to `~/.config/ratana/backups/TIMESTAMP/`.
- Initializes Waybar, default wallpaper (`ratanaos-sequoia-dark.png`), SwayNC, and Kitty.
- Touches `~/.config/ratana/.first-run-complete` to ensure idempotency.
