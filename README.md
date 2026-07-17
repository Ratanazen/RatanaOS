# RatanaOS

RatanaOS is a professional Debian-based Linux distribution under active development.

## Repository Layout

- `kernel/` kernel configuration and patch sets
- `drivers/` hardware enablement and out-of-tree drivers
- `system/` base system configuration and services
- `desktop/` desktop environment integration
- `apps/` first-party applications
- `installer/` installation flow and imaging assets
- `packages/` package definitions and overlays
- `build/` ISO and image build definitions
- `scripts/` automation and developer tooling
- `tests/` validation and release checks
- `docs/` roadmap, architecture, status, changelog, and TODOs
- `branding/` artwork, themes, and release identity

## Workflow

Never push directly to `main`. Work must land through topic branches, validation, and review.

## Desktop Feature

RatanaOS Desktop is a Wayland-first user interface layer built with Qt6 and themed to stay visually aligned with GTK4 surfaces. The current `feature/desktop` implementation adds a desktop shell prototype, core system applications, branding assets, and smoke-test documentation.

### Desktop Components

- `desktop/`: desktop shell, session metadata, and shell entrypoint
- `apps/`: Settings, Software Center, System Monitor, Update Manager, AI Assistant UI, and shared UI helpers
- `branding/`: RatanaOS logo, palette, and GTK4 theme bridge
- `docs/desktop-feature.md`: implementation notes and delivery report
- `tests/smoke_desktop.sh`: repository smoke checks for the desktop feature

### Build

Prerequisites:

- Qt6 Widgets development packages
- CMake 3.21+
- GTK4 development packages and `pkg-config` if you want the optional bridge target

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

### Applications

- `ratana-desktop-shell`
- `ratana-settings`
- `ratana-software-center`
- `ratana-system-monitor`
- `ratana-update-manager`
- `ratana-ai-assistant`
- `ratana-gtk-bridge` (optional)
