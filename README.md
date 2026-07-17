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
