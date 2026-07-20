#!/bin/bash
# Note: VMware script is a placeholder to show invocation as VMware Workstation CLI varies.
set -e

EDITION=${1:-standard}
ISO_FILE="output/RatanaOS-${EDITION}.iso"

echo "VMware integration requires setting up a .vmx file manually or using vmrun."
echo "Ensure vmrun is installed to start VMware VMs automatically."
echo "To test $ISO_FILE, create a VM and mount the ISO."
