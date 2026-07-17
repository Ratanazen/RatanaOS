#!/usr/bin/env bash

set -uo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REPORTS_DIR="${ROOT_DIR}/reports"
REPORT_DATE="$(date +%F)"
REPORT_FILE="${REPORTS_DIR}/release-report-${REPORT_DATE}.md"
ARTIFACT_ISO="${ROOT_DIR}/.artifacts/ratanaos.iso"

mkdir -p "${REPORTS_DIR}"

BRANCH_NAME="$(git -C "${ROOT_DIR}" branch --show-current 2>/dev/null || true)"
if [[ -z "${BRANCH_NAME}" ]]; then
  BRANCH_NAME="unknown"
fi

status_for() {
  if [[ "$1" == "0" ]]; then
    printf "PASS"
  else
    printf "FAIL"
  fi
}

cmd_status() {
  if command -v "$1" >/dev/null 2>&1; then
    printf "present"
  else
    printf "missing"
  fi
}

file_check() {
  if [[ -f "$1" ]]; then
    return 0
  fi

  return 1
}

REMOTE_RC=1
REMOTE_NOTE="origin remote is not configured; git pull origin main failed earlier in this QA run."
if git -C "${ROOT_DIR}" remote get-url origin >/dev/null 2>&1; then
  REMOTE_RC=0
  REMOTE_NOTE="origin remote is configured."
fi

SYNTAX_RC=0
SYNTAX_NOTE="All discovered QA and build scripts parse successfully with bash -n."
SYNTAX_TARGETS=(
  "${ROOT_DIR}/build/build.sh"
  "${ROOT_DIR}/build/test.sh"
  "${ROOT_DIR}/scripts/check-build-deps.sh"
  "${ROOT_DIR}/scripts/configure-live-build.sh"
  "${ROOT_DIR}/scripts/qemu-smoke-test.sh"
  "${ROOT_DIR}/scripts/verify-installer.sh"
  "${ROOT_DIR}/tests/run_release_checks.sh"
)

DISCOVERED_SYNTAX_TARGETS=()
MISSING_SYNTAX_TARGETS=()
for path in "${SYNTAX_TARGETS[@]}"; do
  if [[ -f "${path}" ]]; then
    DISCOVERED_SYNTAX_TARGETS+=("${path}")
  else
    MISSING_SYNTAX_TARGETS+=("${path#${ROOT_DIR}/}")
  fi
done

if [[ "${#DISCOVERED_SYNTAX_TARGETS[@]}" -gt 0 ]]; then
  if ! bash -n "${DISCOVERED_SYNTAX_TARGETS[@]}"; then
    SYNTAX_RC=1
    SYNTAX_NOTE="At least one discovered script failed bash -n validation."
  fi
fi

if [[ "${#MISSING_SYNTAX_TARGETS[@]}" -gt 0 ]]; then
  SYNTAX_RC=1
  SYNTAX_NOTE="Expected pipeline scripts are missing from this branch: ${MISSING_SYNTAX_TARGETS[*]}."
fi

BUILD_ASSETS_RC=0
for path in \
  "${ROOT_DIR}/build/build.sh" \
  "${ROOT_DIR}/scripts/configure-live-build.sh" \
  "${ROOT_DIR}/packages/ratanaos.list.chroot" \
  "${ROOT_DIR}/installer/preseed.cfg" \
  "${ROOT_DIR}/branding/issue"; do
  if ! file_check "${path}"; then
    BUILD_ASSETS_RC=1
  fi
done

BUILD_DEPS_RC=0
for cmd in lb xorriso mksquashfs; do
  if ! command -v "${cmd}" >/dev/null 2>&1; then
    BUILD_DEPS_RC=1
  fi
done

TEST_DEPS_RC=0
for cmd in qemu-system-x86_64 xorriso timeout; do
  if ! command -v "${cmd}" >/dev/null 2>&1; then
    TEST_DEPS_RC=1
  fi
done

BUILD_RC=1
BUILD_NOTE="Build was not executed."
if [[ "${SYNTAX_RC}" -eq 0 && "${BUILD_ASSETS_RC}" -eq 0 && "${BUILD_DEPS_RC}" -eq 0 ]]; then
  if "${ROOT_DIR}/build/build.sh" >/tmp/ratanaos-build.stdout 2>/tmp/ratanaos-build.stderr; then
    BUILD_RC=0
    BUILD_NOTE="build/build.sh completed successfully."
  else
    BUILD_NOTE="build/build.sh executed but failed. See .artifacts/build.log when available."
  fi
else
  BUILD_NOTE="Build prerequisites are incomplete in this environment."
fi

ISO_RC=1
ISO_NOTE="No ISO artifact is available."
if [[ -f "${ARTIFACT_ISO}" ]]; then
  ISO_RC=0
  ISO_NOTE="ISO artifact exists at .artifacts/ratanaos.iso."
fi

INSTALLER_ASSETS_RC=0
for path in "${ROOT_DIR}/installer/preseed.cfg" "${ROOT_DIR}/installer/grub.cfg"; do
  if ! file_check "${path}"; then
    INSTALLER_ASSETS_RC=1
  fi
done

INSTALL_RC=1
INSTALL_NOTE="Installer execution was not tested."
if [[ "${INSTALLER_ASSETS_RC}" -eq 0 && "${ISO_RC}" -eq 0 && "${TEST_DEPS_RC}" -eq 0 ]]; then
  if "${ROOT_DIR}/scripts/verify-installer.sh" "${ARTIFACT_ISO}" >/tmp/ratanaos-installer.stdout 2>/tmp/ratanaos-installer.stderr; then
    INSTALL_RC=0
    INSTALL_NOTE="Installer payload verification passed."
  else
    INSTALL_NOTE="Installer payload verification failed."
  fi
elif [[ "${INSTALLER_ASSETS_RC}" -eq 0 ]]; then
  INSTALL_NOTE="Installer assets exist, but no testable ISO or runtime dependencies are available."
else
  INSTALL_NOTE="Installer assets are incomplete."
fi

BOOT_RC=1
BOOT_NOTE="Boot test was not executed."
if [[ "${ISO_RC}" -eq 0 && "${TEST_DEPS_RC}" -eq 0 ]]; then
  if "${ROOT_DIR}/scripts/qemu-smoke-test.sh" "${ARTIFACT_ISO}" >/tmp/ratanaos-boot.stdout 2>/tmp/ratanaos-boot.stderr; then
    BOOT_RC=0
    BOOT_NOTE="QEMU smoke boot passed."
  else
    BOOT_NOTE="QEMU smoke boot failed."
  fi
else
  BOOT_NOTE="Boot test is blocked by a missing ISO artifact or missing QEMU/xorriso dependencies."
fi

PERF_RC=1
PERF_NOTE="No performance benchmark suite or runnable image is available."

APPS_RC=1
APPS_NOTE="No application smoke tests or application source files were found."
if find "${ROOT_DIR}/apps" -type f 2>/dev/null | grep -q .; then
  APPS_NOTE="Application assets exist, but no smoke test automation is present."
fi
if [[ -x "${ROOT_DIR}/tests/smoke_desktop.sh" ]]; then
  if "${ROOT_DIR}/tests/smoke_desktop.sh" "${ROOT_DIR}" >/tmp/ratanaos-apps.stdout 2>/tmp/ratanaos-apps.stderr; then
    APPS_RC=0
    APPS_NOTE="Desktop and application smoke automation passed via tests/smoke_desktop.sh."
  else
    APPS_NOTE="Desktop smoke automation exists but failed. See /tmp/ratanaos-apps.stderr."
  fi
fi

SECURITY_RC=1
SECURITY_NOTE="Security fixes were not validated; the repo contains package and installer configuration but no security-specific test coverage."

KERNEL_RC=1
KERNEL_NOTE="No kernel configuration or patch files are present under kernel/."
if find "${ROOT_DIR}/kernel" -type f 2>/dev/null | grep -q .; then
  KERNEL_RC=0
  KERNEL_NOTE="Kernel files are present."
fi

DESKTOP_RC=1
DESKTOP_NOTE="No desktop implementation files are present under desktop/."
if find "${ROOT_DIR}/desktop" -type f 2>/dev/null | grep -q .; then
  DESKTOP_RC=0
  DESKTOP_NOTE="Desktop files are present."
fi

DOCS_RC=0
for path in \
  "${ROOT_DIR}/README.md" \
  "${ROOT_DIR}/docs/ARCHITECTURE.md" \
  "${ROOT_DIR}/docs/ROADMAP.md" \
  "${ROOT_DIR}/docs/STATUS.md" \
  "${ROOT_DIR}/docs/TODO.md"; do
  if ! file_check "${path}"; then
    DOCS_RC=1
  fi
done

cat >"${REPORT_FILE}" <<EOF
# RatanaOS Release QA Report

Date: ${REPORT_DATE}
Branch: \`${BRANCH_NAME}\`
QA workflow branch target: \`qa/testing\`

## Scope

- Build
- ISO
- Boot
- Installation
- Performance
- Applications
- Security fixes

## Environment

- Git remote: $(status_for "${REMOTE_RC}") - ${REMOTE_NOTE}
- Shell syntax checks: $(status_for "${SYNTAX_RC}") - ${SYNTAX_NOTE}
- Build dependencies: $(status_for "${BUILD_DEPS_RC}") - lb=$(cmd_status lb), xorriso=$(cmd_status xorriso), mksquashfs=$(cmd_status mksquashfs)
- Test dependencies: $(status_for "${TEST_DEPS_RC}") - qemu-system-x86_64=$(cmd_status qemu-system-x86_64), xorriso=$(cmd_status xorriso), timeout=$(cmd_status timeout)

## Validation Results

- Build: $(status_for "${BUILD_RC}") - ${BUILD_NOTE}
- ISO: $(status_for "${ISO_RC}") - ${ISO_NOTE}
- Boot: $(status_for "${BOOT_RC}") - ${BOOT_NOTE}
- Installation: $(status_for "${INSTALL_RC}") - ${INSTALL_NOTE}
- Performance: $(status_for "${PERF_RC}") - ${PERF_NOTE}
- Applications: $(status_for "${APPS_RC}") - ${APPS_NOTE}
- Security fixes: $(status_for "${SECURITY_RC}") - ${SECURITY_NOTE}

## Release Check

- Kernel $(status_for "${KERNEL_RC}") - ${KERNEL_NOTE}
- Build $(status_for "${BUILD_RC}") - ${BUILD_NOTE}
- Desktop $(status_for "${DESKTOP_RC}") - ${DESKTOP_NOTE}
- Security $(status_for "${SECURITY_RC}") - ${SECURITY_NOTE}
- Documentation $(status_for "${DOCS_RC}") - Core release documentation is present.

## Summary

RatanaOS is not release-ready as of ${REPORT_DATE}. Desktop and application scaffolding now exist, but the broader build and release pipeline remains blocked by missing tools, missing runtime artifacts, and incomplete kernel and security validation.

## Required Next Steps

1. Configure the \`origin\` remote so the QA workflow can synchronize from \`main\`.
2. Install the build and test dependencies required by the existing pipeline.
3. Run \`./build/build.sh\` to produce \`.artifacts/ratanaos.iso\`.
4. Run \`./build/test.sh\` to exercise installer verification and QEMU smoke boot.
5. Expand the current desktop smoke coverage into broader performance, kernel, and security regression tests under \`tests/\`.
EOF

printf 'Release report written to %s\n' "${REPORT_FILE}"

if [[ "${BUILD_RC}" -eq 0 \
  && "${ISO_RC}" -eq 0 \
  && "${BOOT_RC}" -eq 0 \
  && "${INSTALL_RC}" -eq 0 \
  && "${PERF_RC}" -eq 0 \
  && "${APPS_RC}" -eq 0 \
  && "${SECURITY_RC}" -eq 0 \
  && "${KERNEL_RC}" -eq 0 \
  && "${DESKTOP_RC}" -eq 0 \
  && "${DOCS_RC}" -eq 0 ]]; then
  exit 0
fi

exit 1
