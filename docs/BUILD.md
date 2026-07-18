# Building RatanaOS

## Requirements
* Debian 13 Stable (Trixie) or compatible environment
* `live-build`
* root privileges (for lb build)

## Steps to Build
1. Run `make config EDITION=standard`
2. Run `make build EDITION=standard`
3. ISO will be available in `output/`

You can also run `make iso EDITION=standard` to do both steps automatically.
