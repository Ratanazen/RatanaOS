#!/usr/bin/env bash
# ==============================================================================
# RiOS OCI / Docker Container Image Builder (1.0.0)
# Exports the RiOS system rootfs into a standalone Docker / Podman container image
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

BOLD='\033[1m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

IMAGE_TAG="rios:1.0.0"
OUTPUT_TAR="${ROOT_DIR}/build/rios-rootfs.tar.gz"

print_banner() {
    echo -e "${BLUE}${BOLD}"
    echo "╔══════════════════════════════════════════════════════════╗"
    echo "║       RiOS OCI / Docker Container Image Builder          ║"
    echo "║       Export RiOS RootFS to Docker & Podman Image        ║"
    echo "╚══════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

print_banner

echo -e "${CYAN}[1/3] Locating RiOS chroot rootfs...${NC}"
CHROOT_PATH="${ROOT_DIR}/rios-live/chroot"

if [ ! -d "$CHROOT_PATH" ]; then
    echo -e "${YELLOW}Live chroot not extracted. Generating container recipe from Debian Bookworm...${NC}"
    mkdir -p "${ROOT_DIR}/build/container"
    cat << 'DOCKEREOF' > "${ROOT_DIR}/build/container/Dockerfile"
FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive

# Configure RiOS Repositories & Utilities
RUN apt-get update && apt-get install -y --no-install-recommends \
    bash \
    coreutils \
    systemd \
    iproute2 \
    curl \
    wget \
    ca-certificates \
    sudo \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

LABEL org.opencontainers.image.title="RiOS Container Base"
LABEL org.opencontainers.image.version="1.0.0"
LABEL org.opencontainers.image.vendor="RiOS Project"

CMD ["/bin/bash"]
DOCKEREOF

    if command -v docker >/dev/null 2>&1; then
        echo -e "${CYAN}[2/3] Building Docker container image: ${IMAGE_TAG}...${NC}"
        docker build -t "$IMAGE_TAG" -t "rios:latest" -f "${ROOT_DIR}/build/container/Dockerfile" "${ROOT_DIR}/build/container"
        echo -e "\n${GREEN}${BOLD}✔ RiOS Container Image created: ${IMAGE_TAG} and rios:latest${NC}"
        echo -e "Run container: ${BOLD}docker run -it ${IMAGE_TAG}${NC}"
    fi
    exit 0
fi

echo -e "${CYAN}[2/3] Packaging RiOS RootFS into compressed tarball...${NC}"
mkdir -p "${ROOT_DIR}/build"
sudo tar -czf "$OUTPUT_TAR" -C "$CHROOT_PATH" \
    --exclude="proc/*" \
    --exclude="sys/*" \
    --exclude="dev/*" \
    --exclude="tmp/*" \
    --exclude="run/*" \
    . 2>/dev/null || true

echo -e "${CYAN}[3/3] Importing RootFS into Docker / Podman...${NC}"
if command -v docker >/dev/null 2>&1; then
    docker import "$OUTPUT_TAR" "$IMAGE_TAG"
    docker tag "$IMAGE_TAG" "rios:latest"
    echo -e "\n${GREEN}${BOLD}✔ Successfully imported RiOS into Docker: ${IMAGE_TAG}${NC}"
elif command -v podman >/dev/null 2>&1; then
    podman import "$OUTPUT_TAR" "$IMAGE_TAG"
    podman tag "$IMAGE_TAG" "rios:latest"
    echo -e "\n${GREEN}${BOLD}✔ Successfully imported RiOS into Podman: ${IMAGE_TAG}${NC}"
fi

echo -e "Tarball Artifact: ${OUTPUT_TAR} ($(du -h "$OUTPUT_TAR" 2>/dev/null | cut -f1 || echo 'N/A'))"
echo -e "To run container: ${BOLD}docker run -it ${IMAGE_TAG}${NC}"
echo "============================================================"
echo ""
