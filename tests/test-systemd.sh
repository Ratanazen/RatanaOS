#!/usr/bin/env bash
# ==============================================================================
# RiOS Test Suite: Systemd Services, Presets & Enablement Validation
# ==============================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CHROOT_DIR="${ROOT_DIR}/rios-live/config/includes.chroot"
SERVICES_DIR="${CHROOT_DIR}/etc/systemd/system"
PRESETS_DIR="${CHROOT_DIR}/usr/lib/systemd/system-preset"
HOOK_FILE="${ROOT_DIR}/rios-live/config/hooks/live/0130-systemd-enable.hook.chroot"

echo "==> Running Test: test-systemd.sh..."

# 1. Check core systemd unit files
UNITS=(
    "ri-first-boot.service"
    "rios-health.service"
)

for unit in "${UNITS[@]}"; do
    FILE="${SERVICES_DIR}/${unit}"
    if [ ! -f "$FILE" ]; then
        echo "FAIL: Systemd unit file '$FILE' is missing!" >&2
        exit 1
    fi
    
    # Check essential section headers
    for section in "[Unit]" "[Service]" "[Install]"; do
        if ! grep -Fq "${section}" "$FILE"; then
            echo "FAIL: Unit '$unit' is missing required section '$section'!" >&2
            exit 1
        fi
    done
    
    # Check ExecStart command exists in rootfs
    EXEC_START=$(grep -E "^ExecStart=" "$FILE" | head -n1 | cut -d'=' -f2 | awk '{print $1}')
    TARGET_EXEC="${CHROOT_DIR}${EXEC_START}"
    if [ ! -f "$TARGET_EXEC" ] && [ ! -f "${ROOT_DIR}/scripts/$(basename "$EXEC_START")" ]; then
        echo "FAIL: ExecStart target '${EXEC_START}' for unit '$unit' does not exist!" >&2
        exit 1
    fi
    
    echo "  [PASS] Unit validated: $unit (ExecStart: $EXEC_START)"
done

# 2. Check RiOS/services directory synchronization
RIOS_SERVICES=(
    "${ROOT_DIR}/RiOS/services/rios-firstboot.service"
    "${ROOT_DIR}/RiOS/services/rios-health.service"
)

for r_svc in "${RIOS_SERVICES[@]}"; do
    if [ ! -f "$r_svc" ]; then
        echo "FAIL: Service repository file '$r_svc' is missing!" >&2
        exit 1
    fi
    echo "  [PASS] Repo service file: $(basename "$r_svc")"
done

# 3. Check Systemd Preset policy
PRESET_FILE="${PRESETS_DIR}/99-rios.preset"
if [ ! -f "$PRESET_FILE" ]; then
    echo "FAIL: Preset file '$PRESET_FILE' is missing!" >&2
    exit 1
fi

if ! grep -q "enable ri-first-boot.service" "$PRESET_FILE"; then
    echo "FAIL: Preset file does not enable ri-first-boot.service!" >&2
    exit 1
fi
echo "  [PASS] Systemd preset validated: 99-rios.preset"

# 4. Check Chroot Enablement Hook
if [ ! -f "$HOOK_FILE" ]; then
    echo "FAIL: Systemd enablement hook '$HOOK_FILE' is missing!" >&2
    exit 1
fi

if [ ! -x "$HOOK_FILE" ]; then
    echo "FAIL: Systemd enablement hook '$HOOK_FILE' is not executable!" >&2
    exit 1
fi

if ! bash -n "$HOOK_FILE"; then
    echo "FAIL: Systemd enablement hook '$HOOK_FILE' has syntax errors!" >&2
    exit 1
fi
echo "  [PASS] Enablement hook validated: 0130-systemd-enable.hook.chroot"

# 5. Check multi-user.target.wants symlinks
WANTS_DIR="${SERVICES_DIR}/multi-user.target.wants"
if [ ! -d "$WANTS_DIR" ]; then
    echo "FAIL: multi-user.target.wants directory '$WANTS_DIR' is missing!" >&2
    exit 1
fi

if [ ! -L "${WANTS_DIR}/ri-first-boot.service" ]; then
    echo "FAIL: multi-user.target.wants/ri-first-boot.service symlink is missing!" >&2
    exit 1
fi
echo "  [PASS] multi-user.target.wants enablement symlinks verified"

echo "PASS: test-systemd.sh successfully passed all checks!"
