#!/usr/bin/env bash
set -Eeuo pipefail

iso="${1:?usage: smoke-iso.sh PATH-TO-ISO}"
[[ -s "${iso}" ]] || { echo "ISO not found: ${iso}" >&2; exit 1; }
command -v xorriso >/dev/null || { echo 'xorriso is required' >&2; exit 1; }
xorriso -indev "${iso}" -report_el_torito as_mkisofs >/tmp/ratanaos-el-torito.txt 2>&1
grep -Eq -- '-eltorito-boot|-e ' /tmp/ratanaos-el-torito.txt
sha256sum "${iso}"
echo 'ISO boot catalog detected'

