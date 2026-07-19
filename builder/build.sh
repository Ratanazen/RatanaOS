#!/bin/bash
set -e

EDITION=${1:-standard}

echo "Building RatanaOS $EDITION..."

# Pre-build app installation logic
if [ -d "apps" ] && [ "$(ls -A apps)" ]; then
    echo "Staging custom apps..."
    mkdir -p config/includes.chroot/opt/ratanaos-apps
    cp -r apps/* config/includes.chroot/opt/ratanaos-apps/ 2>/dev/null || true
fi

# Run the build
sudo lb build 2>&1 | tee output/build.log

# Move the ISO to output
if ls live-image-amd64.hybrid.iso 1> /dev/null 2>&1; then
    mv live-image-amd64.hybrid.iso output/RatanaOS-${EDITION}.iso
    echo "ISO successfully built and moved to output/RatanaOS-${EDITION}.iso"
    
    # Generate SHA256
    cd output
    sha256sum RatanaOS-${EDITION}.iso > SHA256SUMS
    
    # Print ISO Size
    ls -lh RatanaOS-${EDITION}.iso | awk '{print "ISO Size: "$5}'
    
    # Write report
    echo "# RatanaOS Build Report" > BUILD_REPORT.md
    echo "Edition: $EDITION" >> BUILD_REPORT.md
    echo "Build Date: $(date)" >> BUILD_REPORT.md
    echo "ISO Size: $(du -m RatanaOS-${EDITION}.iso | cut -f1) MB" >> BUILD_REPORT.md
else
    echo "Error: ISO not found after build!"
    exit 1
fi
