# RatanaOS Product Roadmap

> **Versioning Note:** v1.10 in this document means the **10th major release** per internal convention.  
> Standard semantic versioning would label it **v10.0**. Both notations appear below.  
> All releases are **cumulative** — no feature from a prior release is ever removed.

---

## Released

### v1.0.0 – Initial Release (Cyber Edition)
*Foundation: Qt6 desktop, Debian base, 4 build profiles.*

- Custom Qt6 `RatanaUI` widget system
- Qt6 graphical installer (9-step wizard, Btrfs + LUKS)
- Build profiles: `ratana-cyber`, `ratana-developer`, `ratana-lite`, `ratana-workstation`
- AppArmor, UFW, Auditd security stubs
- `RatanaAI Assistant` (mock inference engine, context-aware chat)
- ISO builder with SHA256 checksums and build reports
- CI/CD pipeline (`tests/ci_pipeline.sh`)

---

## In Development

### v5.0 "Phoenix"
*Theme: Multi-architecture, atomic updates, accessibility.*

**New in v5.0:**
- **ARM64 build target** — Raspberry Pi 4/5, generic ARM64 boards
- **Atomic updates with rollback** — Btrfs snapshot-based via `timeshift`/`snapper`
- **Flatpak + Snap** out of the box alongside APT
- **Optional telemetry** — strictly opt-in, anonymized, off by default
- **Accessibility suite** — screen reader, high-contrast themes, magnifier
- **Cloud backup/sync** — optional, user-controlled provider choice
- **RatanaOS Server edition** — headless, ~800 MB ISO, Docker/Podman, automated security patching
- **RatanaOS ARM edition** — GPIO tooling, hardware-optimized kernel, power management

**Installer additions (v5.0):**
- Step 9: Update Strategy (Traditional APT vs. Atomic/Rollback)
- LUKS2 full-disk encryption
- Unattended/scripted install mode (for fleet deployment)

**New applications:**
- `Ratana Assistant` (full AI copilot integration)
- `Ratana Firewall` (GUI front-end for UFW/nftables)
- `Ratana Recovery` (bootable rescue toolkit)

**Build system additions:**
- ARM64 cross-compilation toolchain
- SHA256 + SHA512 checksums
- Software Bill of Materials (SBOM)

**New QA reports:**
- `SECURITY_AUDIT.md`
- `ACCESSIBILITY_REPORT.md`

**New documentation:**
- `SECURITY.md` (vulnerability disclosure policy)
- `ACCESSIBILITY.md`
- `MIGRATION_GUIDE.md` (v4.0 → v5.0 upgrade path)

**New Git branches:** `feature/arm64`, `feature/ai-assistant`, `feature/atomic-updates`

---

## Planned

### v6.0 "Sentinel"
*Theme: Lock down the OS as a hardened, audit-ready platform.*

- **Mandatory Access Control** — AppArmor profiles enabled by default across all editions
- **Full disk encryption by default** on Standard/Developer/Cyber (opt-out, not opt-in)
- **Ratana Firewall: intrusion detection alerts** — fail2ban integration
- **Signed packages + reproducible builds** — build hash verification
- **Automatic security patch channel** — separate from feature updates
- `SECURITY_AUDIT.md` becomes a required CI gate (build fails without passing audit)
- **New Git branch:** `feature/hardening`

---

### v7.0 "Aware"
*Theme: Make the OS proactively helpful, not just AI-assisted on request.*

- **Ratana Assistant gains system-wide context** — diagnoses boot issues, suggests driver fixes, explains error logs in plain language
- **Automated dependency conflict resolution** in Software Center
- **Predictive maintenance** — disk health, thermal, battery degradation warnings
- **Voice command support** — optional, fully local/offline processing (no cloud requirement)
- **Smart update scheduling** — learns user's active hours, avoids interrupting
- **Ratana Style** — new system app for theming (panel layout, accent colors, icon packs, cursor themes, window effects); live preview; import/export `.ratana-theme` profiles
- **New Git branch:** `feature/automation`

---

### v8.0 "Horizon"
*Theme: Fleet management and organizational deployment readiness.*

- **Ratana Fleet Manager** — central dashboard for managing multiple RatanaOS installs (patch status, compliance, inventory)
- **Active Directory / LDAP integration** for Server and Developer editions
- **Zero-touch provisioning** — PXE boot + unattended install profiles for enterprise rollout
- **Multi-tenant user profile isolation**
- **Cloud backup** expands to full system image snapshots (not just user data)
- **New Git branch:** `feature/enterprise`

---

### v9.0 "Convergence"
*Theme: Extend the Ratana identity beyond the traditional desktop.*

- **RatanaOS Mobile Edition** (experimental) — for ARM64 phones/tablets, phone/tablet UI mode for Ratana Desktop
- **Desktop-to-mobile continuity** — clipboard sync, notification mirroring, file handoff
- **Adaptive UI** — fully reflows between desktop, tablet, and phone form factors
- **Ratana Assistant** available as a companion mobile app
- **Hyprland compositor** (optional/experimental) — available for Developer and Cyber editions only; Wayland-native tiling WM with GPU-accelerated animations; marked "experimental" and not under standard stability guarantees
- **New Git branch:** `feature/convergence`

---

### v10.0 "Legacy" *(internal notation: v1.10)*
*Theme: Stabilize everything shipped so far into a durable LTS baseline.*

- **Designated 5-year LTS branch** — frozen core with rolling security patches only
- **Third-party developer SDK** for building native Ratana apps (stable API contract)
- **App Store-style submission/review pipeline** for community apps
- **Full audit of all prior versions' features** for consistency — deprecate anything half-finished
- `ARCHITECTURE.md` and `DEVELOPER_GUIDE.md` rewritten for long-term maintainability
- **New Git branch:** `feature/lts`

---

## Release Checklist (All Versions)

Before any release:
- [ ] Repository clean
- [ ] Documentation updated
- [ ] Tests passed
- [ ] ISO boots successfully (x86_64 and ARM64 where applicable)
- [ ] Installer completes all edition/desktop combos
- [ ] Atomic update + rollback verified (v5.0+)
- [ ] Desktop starts correctly
- [ ] Core applications launch
- [ ] Accessibility features verified (v5.0+)
- [ ] Security audit passed, no critical CVEs open (v6.0+ enforced as CI gate)
- [ ] Checksums and SBOM generated
- [ ] Release notes prepared
- [ ] Migration guide published (for users upgrading from the previous version)

---

## Architectural Overview

```
Hardware (x86_64 + ARM64)
    ↓
Linux Kernel (stable + hardware-optimized ARM profile)
    ↓
GRUB (BIOS/UEFI) + Secure Boot (v6.0+)
    ↓
systemd + initramfs
    ↓
Debian Bookworm base (debootstrap)
    ↓
APT + Flatpak + Snap
    ↓
Atomic Update Layer (Btrfs snapshots, v5.0+)
    ↓
RatanaOS Platform (Qt6 + RatanaUI widget system)
    ↓
Desktop Environment (XFCE / KDE Plasma / GNOME / LXQt / Hyprland v9.0+)
    ↓
Core Applications (Terminal, Settings, Software Center, Update, Monitor, etc.)
    ↓
AI Layer (Ratana Assistant — local inference, v7.0 full context)
    ↓
Live ISO / Installer / ARM Image
```

---

## Edition Summary Table

| Edition | Desktop | ISO Target | Key Additions |
|---|---|---|---|
| Lite | XFCE | ≈2 GB | Lightweight, older hardware |
| Standard | KDE Plasma | ≈3 GB | Everyday desktop |
| Developer | KDE Plasma | ≈3.5 GB | GCC, Rust, Go, Java, Docker, k3s |
| Cyber | KDE Plasma | ≈4 GB | Security toolkits (opt-in categories) |
| Server *(v5.0)* | None (CLI) | ≈800 MB | Hardened kernel, Docker, auto-patching |
| ARM *(v5.0)* | XFCE / KDE | ≈2 GB | GPIO, power mgmt, Raspberry Pi |
| Mobile *(v9.0)* | Adaptive | N/A | Phone/tablet form factor |
