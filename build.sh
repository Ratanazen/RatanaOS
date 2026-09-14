#!/usr/bin/env bash
set -e
if [ "$1" == "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build/*.iso build/*.sha256 rios-live/.build rios-live/binary rios-live/chroot rios-live/cache
    exit 0
fi
echo "Starting RiOS build..."
./tools/fast-live-build.sh
