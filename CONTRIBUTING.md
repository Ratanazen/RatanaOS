# 🤝 Contributing to RatanaOS

Thank you for your interest in contributing to RatanaOS! This document
explains how to build locally, propose changes, and follow project
conventions.

---

## 🛠️ Local Development Setup

### Prerequisites
- **Docker** (Engine or Desktop) — required for containerized live-build.
- **GNU Make** — orchestrates build targets.
- **QEMU** (qemu-system-x86_64) — required for boot testing.
- **Python 3** + **Pillow** — required for icon/theme generation tools.

### Build & Test Cycle
```bash
# Clone
git clone https://github.com/Ratanazen/RatanaOS.git
cd RatanaOS

# Configure live-build
make live-config

# Build ISO
make live-iso

# Boot-test in QEMU
make live-run
```

---

## 📋 Proposing Changes

### Theme / Icon Changes
1. Modify assets in `ratanaos-live/config/includes.chroot/usr/share/themes/`
   or `usr/share/icons/`.
2. Rebuild: `make live-iso`.
3. Boot-test: `make live-run` and verify visually.
4. Submit a PR with before/after screenshots.

### Package List Changes
1. Edit the appropriate list in `ratanaos-live/config/package-lists/`.
2. Verify the package exists in Debian Bookworm:
   ```bash
   docker run --rm ratanaos-live-builder apt-cache show <package-name>
   ```
3. Rebuild and test.

### Hook / Script Changes
- All shell scripts must be **POSIX sh compatible** (no bashisms in
  `.hook.chroot` files — live-build uses `/bin/sh`).
- Run `shellcheck` on any modified scripts before submitting.

---

## 🧪 Testing Requirements
- Every PR that changes package lists, themes, or hooks **must** include
  confirmation that the contributor built and booted the ISO locally.
- Reference `docs/PARITY_CHECKLIST.md` for the full QA verification list.

---

## 💬 Code Style
- **Shell scripts**: POSIX sh, `set -e`, shellcheck-clean.
- **Python tools**: Python 3.9+, PEP 8 formatting.
- **Commit messages**: Conventional Commits format —
  `feat(phase-N): description`, `fix(theme): description`, etc.
- **One logical change per commit** — do not bundle unrelated changes.
