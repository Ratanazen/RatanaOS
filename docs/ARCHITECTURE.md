# RatanaOS Architecture

This repository is organized around core OS layers: kernel, drivers, system services, installer, packages, desktop, applications, branding, and validation tooling.

The desktop feature introduces a UI layer composed of:

- Qt6 desktop shell and first-party system applications
- optional GTK4 bridge target and theme tokens
- Wayland session metadata for compositor-facing desktop startup
- shared branding assets and smoke-test coverage
