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
check grep -q '@VERSION@' "${root}/branding/etc/os-release"
check grep -q '^task-kde-desktop$' "${root}/packages/lists/desktop.list.chroot"

version="$(tr -d '[:space:]' < "${root}/VERSION")"
check bash -c '[[ "$1" =~ ^[0-9]+\.[0-9]+\.[0-9]+([.-][A-Za-z0-9]+)*$ ]]' _ "${version}"

source "${root}/scripts/lib.sh"
rendered="$(mktemp)"
trap 'rm -f "${rendered}"' EXIT
render_template "${root}/branding/etc/os-release" "${rendered}"
check grep -q "^VERSION=\"${version}\"$" "${rendered}"
check bash -c '! grep -q "@VERSION" "$1"' _ "${rendered}"

for script in "${root}"/scripts/*.sh "${root}"/tests/*.sh; do
  check bash -n "${script}"
done

(( failures == 0 )) || { printf '%d test(s) failed\n' "${failures}" >&2; exit 1; }
