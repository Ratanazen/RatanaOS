#!/usr/bin/env bash
set -Eeuo pipefail

project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
iso="${1:-}"

"${project_root}/tests/run.sh"

if [[ -z "${iso}" ]]; then
  printf '%s\n' 'Static validation passed. Pass an ISO path to run ISO and QEMU checks.'
  exit 0
fi

[[ -f "${iso}" ]] || { printf 'ISO not found: %s\n' "${iso}" >&2; exit 1; }
"${project_root}/tests/smoke-iso.sh" "${iso}"

command -v qemu-system-x86_64 >/dev/null 2>&1 || {
  printf '%s\n' 'QEMU not found; ISO validation passed but VM boot test was skipped.' >&2
  exit 2
}

printf '%s\n' 'Starting QEMU. Close the VM after verifying the live boot menu and desktop.'
exec qemu-system-x86_64 \
  -m "${QEMU_MEMORY:-4096}" \
  -smp "${QEMU_CPUS:-2}" \
  -enable-kvm \
  -boot d \
  -cdrom "${iso}"
