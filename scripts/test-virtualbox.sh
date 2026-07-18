#!/bin/bash
set -e

EDITION=${1:-standard}
ISO_FILE="output/RatanaOS-${EDITION}.iso"
VM_NAME="RatanaOS_Test"

if [ ! -f "$ISO_FILE" ]; then
    echo "Error: $ISO_FILE not found!"
    exit 1
fi

echo "Setting up VirtualBox VM..."
if VBoxManage showvminfo "$VM_NAME" &>/dev/null; then
    VBoxManage unregistervm "$VM_NAME" --delete
fi

VBoxManage createvm --name "$VM_NAME" --ostype "Debian_64" --register
VBoxManage modifyvm "$VM_NAME" --memory 4096 --cpus 4 --vram 128 --graphicscontroller vmsvga
VBoxManage storagectl "$VM_NAME" --name "IDE" --add ide
VBoxManage storageattach "$VM_NAME" --storagectl "IDE" --port 0 --device 0 --type dvddrive --medium "$ISO_FILE"

echo "Starting VirtualBox VM..."
VBoxManage startvm "$VM_NAME"
