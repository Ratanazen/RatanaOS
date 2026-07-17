# RatanaOS Hardening Baseline

Date: 2026-07-17

## Implemented In This Branch

- AppArmor baseline and two initial confined profiles
- Secure Boot design requirements and hardened kernel command line
- `nftables` default-drop inbound and forward policy
- System-wide sysctl hardening
- Permission management defaults for login and service-owned state
- Security package manifest for image builds
- Baseline security validation script

## Integration Requirements

1. Install `packages/manifests/security-base.list` in the base image.
2. Copy `system/firewall/nftables.conf` into `/etc/nftables.conf` and enable the `nftables` service.
3. Apply `system/security/sysctl.d/50-ratanaos-security.conf` into `/etc/sysctl.d/`.
4. Apply `system/security/permissions/login.defs` and `tmpfiles.conf` during image creation.
5. Install AppArmor profiles into `/etc/apparmor.d/` and enable the AppArmor service.
6. Propagate `system/boot/secure-boot/kernel-cmdline.conf` into the bootloader configuration for signed images.

## Open Follow-Up

- Add APT repository signing and key rotation policy.
- Add Polkit policy review before any GUI administrative application ships.
- Add service accounts and file ownership manifests as soon as daemons land.
- Replace temporary SSH firewall allowance if desktop images should not expose SSH by default.

