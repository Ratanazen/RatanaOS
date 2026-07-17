# RatanaOS project status

Last updated: 2026-07-17

## Release gates

| Gate | Status | Evidence / blocker |
|---|---|---|
| Security audit report | Approved | `docs/security/audit-2026-07-17.md`, commit `06aeaf4` |
| Security remediation | Pending | Medium findings M1–M5 remain open |
| Build validation | Pending | CI environment and full ISO build require validation |
| Merge to `main` | Blocked | Requires both build and security approval |

The audit report is accepted as an accurate review of the Phase 1 source at
commit `a6e1b80`. This approval accepts the report, not the current image for
production release. No merge is authorized until the build pipeline passes and
the applicable security findings are fixed, retested, and approved.

## Assigned work

### Terminal 1 — Development

Continue the current development milestone. Keep new privileged code, PolicyKit
rules, installer changes, kernel patches, drivers, and dependency additions
separately reviewable and submit them for security review before integration.

### Terminal 2 — Build and CI

Fix and validate the build CI environment. Prioritize reproducible build inputs,
pinned GitHub Actions and container references, reduced job privileges, package
manifests, and successful static, ISO, BIOS, and UEFI checks. Provide logs and
artifact hashes for approval.

### Terminal 3 — Research

Research the Debian desktop stack. Document supported KDE/Plasma, display
manager, networking, update, firewall, AppArmor, firmware, installer, and Secure
Boot components, including lifecycle and security implications. Do not treat
research recommendations as approved dependencies until reviewed.

### Terminal 4 — Security

Track remediation of findings M1–M5, review CI changes from Terminal 2, and
repeat source and generated-image checks before granting the security gate.

## Approval policy

- Do not merge directly to `main`.
- Do not approve an ISO based only on source-level static checks.
- Build approval requires a successful clean build and retained validation
  evidence.
- Security approval requires remediation or documented acceptance of applicable
  findings and verification of the generated ISO.
