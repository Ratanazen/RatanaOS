#!/usr/bin/env bash
set -Eeuo pipefail
source "$(dirname "$0")/lib.sh"

require_command lb
mkdir -p "${WORK_DIR}"
cd "${WORK_DIR}"

if [[ -d .build ]]; then
  lb clean --purge
fi

log "configuring ${SUITE}/${ARCH} live image"
lb config \
  --mode debian \
  --distribution "${SUITE}" \
  --architectures "${ARCH}" \
  --archive-areas "main contrib non-free-firmware" \
  --mirror-bootstrap "${MIRROR}" \
  --mirror-chroot "${MIRROR}" \
  --mirror-binary "${MIRROR}" \
  --debian-installer live \
  --debian-installer-gui true \
  --bootappend-live "boot=live components quiet splash username=ratana hostname=ratanaos locales=en_US.UTF-8 keyboard-layouts=us" \
  --image-name "ratanaos-${VERSION}-${ARCH}" \
  --iso-application "RatanaOS Live" \
  --iso-publisher "RatanaOS Project" \
  --iso-volume "RATANAOS_${VERSION:0:12}" \
  --checksums sha256 \
  --memtest none

rm -rf config/package-lists config/includes.chroot config/hooks
mkdir -p config/package-lists config/includes.chroot/etc config/hooks/live
cp "${PROJECT_ROOT}/packages/lists/desktop.list.chroot" config/package-lists/
render_template "${PROJECT_ROOT}/branding/etc/os-release" config/includes.chroot/etc/os-release
render_template "${PROJECT_ROOT}/branding/etc/issue" config/includes.chroot/etc/issue
render_template "${PROJECT_ROOT}/branding/etc/issue.net" config/includes.chroot/etc/issue.net
render_template \
  "${PROJECT_ROOT}/build/config/hooks/010-ratana-identity.hook.chroot" \
  config/hooks/live/010-ratana-identity.hook.chroot
chmod 0755 config/hooks/live/010-ratana-identity.hook.chroot
log "configuration generated in ${WORK_DIR}"
