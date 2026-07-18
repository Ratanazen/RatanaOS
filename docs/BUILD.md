# RatanaOS Build Guide

To build the RatanaOS Live ISO, you must be on a Debian-based host system (or a mock development environment).

## Requirements
- `debootstrap`
- `live-build`
- `xorriso`
- `mksquashfs`

## Steps
1. Navigate to the repository root.
2. Run `./tests/ci_pipeline.sh` for an automated build and test.
3. Or manually build the ISO:
   ```bash
   ./builder/build-iso.sh ratana-cyber
   ```
4. Find the resulting ISO in `output/RatanaOS-amd64.iso`.
