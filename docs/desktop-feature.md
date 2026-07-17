# Desktop Feature Report

## Scope

The `feature/desktop` implementation establishes a RatanaOS desktop UI foundation using Qt6 for the shell and primary system applications, with GTK4 styling support and Wayland session metadata for desktop integration.

## UI Changes

- Added a Wayland-first desktop shell with a persistent navigation rail, overview cards, quick-action space, and daily agenda surface.
- Introduced a shared RatanaOS visual language: warm sandstone gradients, elevated panels, rounded cards, and a unified typography stack.
- Added application-specific layouts for Settings, Software Center, System Monitor, Update Manager, and AI Assistant UI.
- Added optional GTK4 bridge styling to keep GTK-based applications aligned with the same branding tokens.

## Applications Created

- `ratana-desktop-shell`: desktop environment shell prototype
- `ratana-settings`: personalization and system settings workspace
- `ratana-software-center`: application discovery and install surface
- `ratana-system-monitor`: process and performance dashboard
- `ratana-update-manager`: unified package and firmware update workflow
- `ratana-ai-assistant`: chat-based assistant surface for automation and explanations
- `ratana-gtk-bridge`: optional GTK4 validation target

## Repository Layout

- `desktop/src/main.cpp`: desktop shell entry point
- `desktop/session/ratanaos-wayland.desktop`: Wayland session metadata
- `apps/common/`: shared UI toolkit and optional GTK4 bridge
- `apps/*/main.cpp`: individual application entry points
- `branding/`: logo, palette, and GTK theme bridge
- `tests/smoke_desktop.sh`: repository smoke validation

## Build Notes

Use the following commands once the required toolchain is installed:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Current environment limitation on July 17, 2026:

- `cmake` is not installed
- `pkg-config` is not installed

As a result, compile validation could not be executed in the current shell session.
