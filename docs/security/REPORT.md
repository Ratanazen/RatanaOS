# Security Report

Date: 2026-07-17
Branch: `security/audit`

Critical:
- Secure Boot trust chain is not yet backed by signed EFI, GRUB, and kernel artifacts in this repository.
- Package signing and update trust infrastructure do not exist yet.

High:
- No prior firewall defaults existed; this branch adds a default-drop `nftables` baseline.
- No prior AppArmor confinement existed; this branch adds initial mandatory access control scaffolding.
- No prior permission-management baseline existed; this branch adds restrictive login and tmpfiles defaults.

Medium:
- No kernel hardening sysctl baseline existed before this branch.
- No automated security baseline checks existed before this branch.
- Security dependencies were not declared before this branch.

Low:
- No application or driver source exists yet, so unsafe-code review and CVE triage are deferred.

Recommendations:
- Enforce signed EFI, GRUB, kernel, and module artifacts in the image pipeline before alpha.
- Implement APT repository signing, key rotation, and update metadata verification before publishing packages.
- Remove the temporary SSH ingress allowance from `system/firewall/nftables.conf` unless remote admin is a product requirement.
- Add service users, Polkit policy review, and per-package file ownership manifests as daemons and GUI admin tools are introduced.
- Add CI jobs to run `tests/security/audit-baseline.sh` on every security, build, and installer branch.
