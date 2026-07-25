#!/bin/bash
set -e
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EDITION="${1:-standard}"
exec "${ROOT_DIR}/scripts/test-qemu.sh" "${EDITION}"
