# 📀 RatanaOS Image Variants (Phase 10)

## Defined Variants

### 1. `ratanaos-desktop` (Primary — Default Build)
- **Audience**: End-users, developers, and designers.
- **Includes**: Full XFCE4 desktop, Plank dock, macOS Sequoia theme layer,
  LightDM, Calamares installer, Firefox ESR, PipeWire audio, full hardware
  firmware, Khmer/multilingual locale support, printing (CUPS), and
  developer tooling (`git`, `build-essential`, etc.).
- **Build**: `make iso-desktop` (equivalent to `make live-iso`).
- **Approximate Size**: ~2.5–3.5 GB (full live hybrid ISO).

### 2. `ratanaos-minimal` (CLI-Only Base)
- **Audience**: Server operators, container base images, VM appliances, and
  advanced users who want to build their own desktop stack.
- **Includes**: Debian Bookworm base system, systemd, NetworkManager,
  full hardware firmware, SSH server **disabled** by default, `sudo`,
  `curl`, `vim`, `htop`. No desktop environment, no GUI.
- **Excludes**: XFCE4, LightDM, Plank, Calamares, Firefox, PipeWire,
  CUPS, and all GUI-related packages.
- **Build**: `make iso-minimal`.
- **Approximate Size**: ~600–900 MB.

### 3. `ratanaos-server` (Headless Server)
- **Audience**: Web servers, development servers, self-hosted services.
- **Includes**: Everything in `ratanaos-minimal` plus `openssh-server`
  (enabled by default), `ufw` (with default deny incoming), `nginx-light`,
  `fail2ban`, `unattended-upgrades`.
- **Excludes**: All GUI packages.
- **Build**: `make iso-server`.
- **Approximate Size**: ~700–1000 MB.

---

## Build Targets

| Make Target | Variant | Description |
| :--- | :--- | :--- |
| \`make iso-desktop\` | Desktop | Full themed XFCE desktop (default) |
| \`make iso-minimal\` | Minimal | CLI-only base system |
| \`make iso-server\` | Server | Headless server with SSH + nginx |
| \`make iso-all\` | All | Builds all three variants sequentially |

---

## Current Scope Decision
**Only `ratanaos-desktop` is actively built and tested.** The `minimal`
and `server` variants are defined as future targets. Their build
infrastructure (separate \`lb config\` profiles under \`config-minimal/\`
and \`config-server/\`) will be created when explicitly requested.
