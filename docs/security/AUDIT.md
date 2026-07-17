# RatanaOS Security Audit

Date: 2026-07-17
Branch: `security/audit`
Auditor: Security Engineer

## Scope

- Kernel
- Drivers
- Packages
- Applications
- Permissions
- Boot system

## Summary

The repository is still in bootstrap state with no kernel patches, package recipes, applications, installer logic, or boot assets checked in. That means the highest current security risk is absence of implementation and validation rather than confirmed exploitable code inside the tree. This audit adds the initial hardening baseline so future development starts from secure defaults instead of retrofitting them later.

## Findings

### Critical

1. No secure boot chain is implemented yet.
   - Impact: Future images could boot unsigned EFI, GRUB, or kernel artifacts if release tooling does not enforce signatures.
   - Status: Mitigated in design by `system/boot/secure-boot/README.md` and `system/boot/secure-boot/kernel-cmdline.conf`, but not yet validated on hardware.

2. No package signing or update trust path is implemented yet.
   - Impact: Repository metadata and update delivery could be spoofed once package infrastructure exists.
   - Status: Partially mitigated by requiring `gpgv` and keyring access boundaries in the update agent AppArmor profile; full APT repository signing still missing.

### High

1. No firewall or network exposure policy existed.
   - Impact: Early services could ship with permissive inbound exposure.
   - Status: Mitigated by `system/firewall/nftables.conf` default-drop inbound and forward policy.

2. No mandatory access control baseline existed.
   - Impact: First-party apps and installer components would run with unrestricted DAC-only permissions.
   - Status: Mitigated by AppArmor baseline and initial profiles for installer and update workflows.

3. No permission management baseline existed.
   - Impact: Services and users could inherit weak umask and writable-state defaults.
   - Status: Mitigated by `login.defs`, tmpfiles policy, and least-privilege guidance.

### Medium

1. No kernel hardening sysctls were defined.
   - Impact: Desktop images could allow weaker network and introspection defaults.
   - Status: Mitigated by `system/security/sysctl.d/50-ratanaos-security.conf`.

2. No security validation checks existed.
   - Impact: Hardening regressions could be introduced without detection.
   - Status: Mitigated by `tests/security/audit-baseline.sh`.

3. No dependency baseline was defined for security features.
   - Impact: Builds may omit AppArmor, shim, signing, or firewall packages.
   - Status: Mitigated by `packages/manifests/security-base.list`.

### Low

1. No application code is present to audit for unsafe patterns yet.
   - Impact: Static analysis of memory safety and injection risks cannot begin.
   - Status: Open until `apps/` and `system/` contain executable code.

2. No out-of-tree drivers are present to review.
   - Impact: Driver attack-surface review is deferred.
   - Status: Open until driver sources or packaging land.

## Area Review

### Kernel

- No kernel configuration or patch queue exists yet.
- Added baseline kernel command-line hardening for AppArmor, lockdown, module signature enforcement, and memory safety posture.

### Drivers

- `drivers/` is empty.
- No third-party or vendor code is present to assess for taint, signing, or maintenance risk.

### Packages

- `packages/` was empty.
- Added `packages/manifests/security-base.list` to pin mandatory hardening dependencies for future images.

### Applications

- `apps/` is empty.
- Added placeholder AppArmor profiles for planned installer and update agent entrypoints to enforce confinement from first implementation.

### Permissions

- No prior account, group, or file ownership policy existed.
- Added login and tmpfiles baselines plus least-privilege guidance.

### Boot System

- No bootloader, signing, or Secure Boot assets were present.
- Added Secure Boot support requirements and a hardened kernel command-line baseline.

## Checks Performed

- Repository structure review
- Documentation review
- Presence validation for security baseline artifacts
- Configuration inspection for firewall, kernel command line, sysctl, and permission defaults

## Not Yet Possible

- Vulnerability scanning against built packages
- Binary analysis
- Secure Boot boot test on hardware or VM firmware
- Service exposure validation on a running image
- Dependency CVE triage for final package set

