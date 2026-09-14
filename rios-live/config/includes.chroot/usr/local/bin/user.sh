#!/usr/bin/env bash
# ==============================================================================
# RiOS Installer: User & Authentication Configuration Module
# ==============================================================================
set -euo pipefail

configure_user() {
    local target_mount="${1:-/mnt/target}"
    local username="${2:-ri}"
    local password="${3:-ri}"
    local fullname="${4:-Ri}"
    local root_password="${5:-$password}"

    echo "==> Configuring primary user '$username' on target system..."

    # Check if user already exists
    if chroot "$target_mount" id -u "$username" >/dev/null 2>&1; then
        echo "User '$username' already exists. Updating password..."
        echo "${username}:${password}" | chroot "$target_mount" chpasswd
    else
        echo "Creating user '$username' ($fullname)..."
        chroot "$target_mount" useradd -m -s /bin/bash -c "$fullname" "$username"
        echo "${username}:${password}" | chroot "$target_mount" chpasswd
    fi

    # Set root password
    echo "root:${root_password}" | chroot "$target_mount" chpasswd

    # Add user to hardware and administrative groups
    local groups="sudo,audio,video,plugdev,netdev,input"
    for grp in sudo audio video plugdev netdev input; do
        chroot "$target_mount" groupadd -f "$grp" >/dev/null 2>&1 || true
    done
    chroot "$target_mount" usermod -aG "$groups" "$username"

    # Grant sudo privileges without password if specified, or standard sudo
    mkdir -p "${target_mount}/etc/sudoers.d"
    echo "${username} ALL=(ALL:ALL) ALL" > "${target_mount}/etc/sudoers.d/90-${username}"
    chmod 0440 "${target_mount}/etc/sudoers.d/90-${username}"

    # Copy skeleton config files
    if [ -d "${target_mount}/etc/skel" ]; then
        echo "Deploying skeleton configuration to /home/${username}..."
        cp -rT "${target_mount}/etc/skel" "${target_mount}/home/${username}"
        chroot "$target_mount" chown -R "${username}:${username}" "/home/${username}"
    fi

    echo "User configuration completed for '${username}'."
}

if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    configure_user "${1:-/mnt/target}" "${2:-ri}" "${3:-ri}" "${4:-Ri}" "${5:-ri}"
fi
