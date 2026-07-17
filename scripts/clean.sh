#!/usr/bin/env bash
set -Eeuo pipefail
source "$(dirname "$0")/lib.sh"

if [[ -d "${WORK_DIR}" ]] && command -v lb >/dev/null 2>&1; then
  (cd "${WORK_DIR}" && lb clean --purge) || true
fi
log "generated live-build state cleaned; release artifacts retained"

