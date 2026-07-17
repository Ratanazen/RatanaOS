#!/usr/bin/env bash
set -Eeuo pipefail
source "$(dirname "$0")/lib.sh"

[[ ${EUID} -eq 0 ]] || die "ISO builds require root (live-build mounts filesystems)"
for command in lb debootstrap mksquashfs xorriso; do require_command "${command}"; done

"${PROJECT_ROOT}/scripts/configure-build.sh"
cd "${WORK_DIR}"
log "building ISO; this can take a while"
lb build

iso="$(find . -maxdepth 1 -type f -name '*.iso' -print -quit)"
[[ -n "${iso}" ]] || die "live-build completed without producing an ISO"
mkdir -p "${ARTIFACT_DIR}"
destination="${ARTIFACT_DIR}/ratanaos-${VERSION}-${ARCH}.iso"
cp "${iso}" "${destination}"
(cd "${ARTIFACT_DIR}" && sha256sum "$(basename "${destination}")" > "$(basename "${destination}").sha256")
log "created ${destination}"

