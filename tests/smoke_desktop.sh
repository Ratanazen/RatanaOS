#!/usr/bin/env bash
set -euo pipefail

root="${1:-$(pwd)}"

required_files=(
  "$root/CMakeLists.txt"
  "$root/desktop/src/main.cpp"
  "$root/desktop/session/ratanaos-wayland.desktop"
  "$root/apps/settings/main.cpp"
  "$root/apps/software-center/main.cpp"
  "$root/apps/system-monitor/main.cpp"
  "$root/apps/update-manager/main.cpp"
  "$root/apps/ai-assistant/main.cpp"
  "$root/apps/common/ratana_ui.cpp"
  "$root/branding/logo.svg"
  "$root/branding/palette.json"
  "$root/docs/desktop-feature.md"
)

for file in "${required_files[@]}"; do
  [[ -f "$file" ]] || {
    echo "Missing required file: $file" >&2
    exit 1
  }
done

grep -q "ratana-desktop-shell" "$root/CMakeLists.txt" "$root/desktop/CMakeLists.txt"
grep -q "RatanaOS Desktop" "$root/desktop/src/main.cpp"
grep -q "Software Center" "$root/apps/software-center/main.cpp"
grep -q "System Monitor" "$root/apps/system-monitor/main.cpp"
grep -q "Update Manager" "$root/apps/update-manager/main.cpp"
grep -q "AI Assistant" "$root/apps/ai-assistant/main.cpp"

echo "RatanaOS desktop smoke test passed."
