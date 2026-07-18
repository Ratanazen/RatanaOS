# Building RatanaOS (v16.0)

RatanaOS uses a modular asset architecture. The build system automatically scans your local directories and injects them into the final ISO.

## Directory Structure
- `assets/wallpapers/`: Drop your `.jpg` or `.png` files here. The installer and Appearance Center will find them automatically.
- `assets/icons/`: Drop your icon packs here.
- `branding/grub/`: Edit the GRUB background or theme here.
- `config/ratanaos-config.yaml`: The central configuration engine. Define default themes, wallpapers, and branding targets here.

## How to Build

1. Validate your `ratanaos-config.yaml`
2. Run the build script:
```bash
./builder/build-iso.sh ratana-standard
```

The script will automatically halt if the assets defined in your configuration are missing from the `assets/` directory.

## Testing

You can use the provided VM test scripts:
```bash
make vm EDITION=ratana-standard
```
