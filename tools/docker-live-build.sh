#!/usr/bin/env bash
# ==============================================================================
# RatanaOS — Dockerized Debian Live-Build Runner
# Provides a pure Debian Bookworm environment for live-build on any host.
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
LIVE_DIR="${ROOT_DIR}/ratanaos-live"
IMAGE_NAME="ratanaos-live-builder"

mkdir -p "${LIVE_DIR}"
mkdir -p "${ROOT_DIR}/build"

build_docker_image() {
    echo "==> Verifying Debian live-build container image..."
    cat << 'DOCKEREOF' | docker build -t "${IMAGE_NAME}" -
FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    live-build \
    debootstrap \
    squashfs-tools \
    xorriso \
    isolinux \
    syslinux-common \
    syslinux-efi \
    grub-pc-bin \
    grub-efi-amd64-bin \
    mtools \
    dosfstools \
    ca-certificates \
    git \
    rsync \
    curl \
    wget \
    cpio \
    fdisk \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace/ratanaos-live
DOCKEREOF
}

run_in_container() {
    docker run --rm -it \
        --privileged \
        --net=host \
        -v "${ROOT_DIR}:/workspace" \
        -w /workspace/ratanaos-live \
        "${IMAGE_NAME}" \
        "$@"
}

run_in_container_non_interactive() {
    docker run --rm \
        --privileged \
        --net=host \
        -v "${ROOT_DIR}:/workspace" \
        -w /workspace/ratanaos-live \
        "${IMAGE_NAME}" \
        "$@"
}

ACTION="${1:-build}"

case "${ACTION}" in
    image)
        build_docker_image
        ;;
    config)
        build_docker_image
        echo "==> Configuring live-build inside container..."
        run_in_container_non_interactive lb config \
            --distribution bookworm \
            --archive-areas "main contrib non-free non-free-firmware" \
            --debian-installer live \
            --iso-application "RatanaOS" \
            --iso-volume "RatanaOS" \
            --binary-images iso-hybrid \
            --bootloader syslinux,grub-efi
        ;;
    build)
        build_docker_image
        echo "==> Running live-build inside container..."
        run_in_container_non_interactive lb build
        if ls "${LIVE_DIR}"/*.iso 1>/dev/null 2>&1; then
            cp "${LIVE_DIR}"/*.iso "${ROOT_DIR}/build/ratanaos-live-amd64.hybrid.iso"
            echo "==> Live ISO generated at ${ROOT_DIR}/build/ratanaos-live-amd64.hybrid.iso"
        fi
        ;;
    clean)
        build_docker_image
        echo "==> Cleaning live-build tree..."
        run_in_container_non_interactive lb clean --purge
        ;;
    shell)
        build_docker_image
        run_in_container bash
        ;;
    *)
        echo "Usage: $0 {image|config|build|clean|shell}"
        exit 1
        ;;
esac
