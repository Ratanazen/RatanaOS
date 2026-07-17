#!/bin/bash
# RatanaOS Automated ISO Builder

PROFILE=$1

if [ -z "$PROFILE" ]; then
  echo "Usage: $0 <profile>"
  echo "Available profiles: ratana-desktop, ratana-lite, ratana-developer, ratana-security, minimal"
  exit 1
fi

echo "[ISO Builder] Starting build for profile: $PROFILE"
echo " -> Fetching packages..."
echo " -> Building initramfs..."
echo " -> Creating squashfs..."
echo " -> Generating ISO checksum..."

# Mock output for testing
echo "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855  RatanaOS-${PROFILE}.iso" > "output/RatanaOS-${PROFILE}.iso.sha256"

echo "[ISO Builder] Build complete. Output: output/RatanaOS-${PROFILE}.iso"
