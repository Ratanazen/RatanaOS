#!/usr/bin/env bash
set -Eeuo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WORK_DIR="${PROJECT_ROOT}/build/work"
ARTIFACT_DIR="${PROJECT_ROOT}/build/artifacts"
ARCH="${ARCH:-amd64}"
SUITE="${SUITE:-stable}"
MIRROR="${MIRROR:-https://deb.debian.org/debian}"
VERSION="$(tr -d '[:space:]' < "${PROJECT_ROOT}/VERSION")"
VERSION_ID="${VERSION%%-*}"

[[ "${VERSION}" =~ ^[0-9]+\.[0-9]+\.[0-9]+([.-][A-Za-z0-9]+)*$ ]] || {
  printf '[RatanaOS] ERROR: invalid VERSION: %s\n' "${VERSION}" >&2
  exit 1
}

log() { printf '[RatanaOS] %s\n' "$*"; }
die() { printf '[RatanaOS] ERROR: %s\n' "$*" >&2; exit 1; }
require_command() { command -v "$1" >/dev/null 2>&1 || die "missing command: $1"; }

render_template() {
  local source="$1" destination="$2"
  sed -e "s/@VERSION@/${VERSION}/g" -e "s/@VERSION_ID@/${VERSION_ID}/g" \
    "${source}" > "${destination}"
}
