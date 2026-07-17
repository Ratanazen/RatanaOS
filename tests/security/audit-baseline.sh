#!/usr/bin/env bash
set -euo pipefail

required_files=(
  "packages/manifests/security-base.list"
  "system/apparmor/README.md"
  "system/apparmor/profiles/usr.bin.ratana-installer"
  "system/apparmor/profiles/usr.libexec.ratana-update-agent"
  "system/boot/secure-boot/README.md"
  "system/boot/secure-boot/kernel-cmdline.conf"
  "system/firewall/nftables.conf"
  "system/security/sysctl.d/50-ratanaos-security.conf"
  "system/security/permissions/login.defs"
  "system/security/permissions/tmpfiles.conf"
  "docs/security/AUDIT.md"
  "docs/security/REPORT.md"
)

for file in "${required_files[@]}"; do
  if [[ ! -f "$file" ]]; then
    echo "missing: $file" >&2
    exit 1
  fi
done

grep -Eq '^[[:space:]]*policy drop;' system/firewall/nftables.conf
grep -q '^apparmor$' <(tr ',=' '\n\n' < system/boot/secure-boot/kernel-cmdline.conf)
grep -q '^module.sig_enforce=1$' system/boot/secure-boot/kernel-cmdline.conf
grep -q '^UMASK 027$' system/security/permissions/login.defs
grep -q '^kernel.unprivileged_bpf_disabled = 1$' system/security/sysctl.d/50-ratanaos-security.conf

echo "security baseline audit checks passed"
