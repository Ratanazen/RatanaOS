#!/bin/bash
# RatanaOS Kernel Integration Script
# Applies latest stable Linux kernel optimizations

echo "[Kernel Integration] Initializing..."

# 1. Hardware compatibility & drivers
echo " -> Loading WiFi/Bluetooth modules..."
modprobe iwlwifi || echo "iwlwifi not found, skipping"
modprobe btusb || echo "btusb not found, skipping"

# 2. CPU / GPU optimization
echo " -> Setting CPU frequency scaling governor to 'schedutil'..."
for cpu in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
    echo "schedutil" > "$cpu" 2>/dev/null
done

# 3. Power management
echo " -> Applying PCIe ASPM power saving..."
echo "powersave" > /sys/module/pcie_aspm/parameters/policy 2>/dev/null

echo "[Kernel Integration] Complete."
