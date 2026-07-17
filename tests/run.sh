#!/usr/bin/env bash
set -Eeuo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
failures=0

check() {
  if "$@"; then printf 'ok - %s\n' "$*"; else printf 'not ok - %s\n' "$*"; failures=$((failures + 1)); fi
}

check test -s "${root}/VERSION"
check test -x "${root}/scripts/build-iso.sh"
check test -x "${root}/scripts/configure-build.sh"
check grep -q '^ID=ratanaos$' "${root}/branding/etc/os-release"
check grep -q '^ID_LIKE=debian$' "${root}/branding/etc/os-release"
check grep -q '^task-kde-desktop$' "${root}/packages/lists/desktop.list.chroot"

for script in "${root}"/scripts/*.sh "${root}"/tests/*.sh; do
  check bash -n "${script}"
done

(( failures == 0 )) || { printf '%d test(s) failed\n' "${failures}" >&2; exit 1; }

