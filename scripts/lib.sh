#!/usr/bin/env bash
set -Eeuo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WORK_DIR="${PROJECT_ROOT}/build/work"
ARTIFACT_DIR="${PROJECT_ROOT}/build/artifacts"
ARCH="${ARCH:-amd64}"
SUITE="${SUITE:-stable}"
MIRROR="${MIRROR:-https://deb.debian.org/debian}"
VERSION="$(tr -d '[:space:]' < "${PROJECT_ROOT}/VERSION")"

log() { printf '[RatanaOS] %s\n' "$*"; }
die() { printf '[RatanaOS] ERROR: %s\n' "$*" >&2; exit 1; }
require_command() { command -v "$1" >/dev/null 2>&1 || die "missing command: $1"; }

