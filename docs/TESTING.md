# Testing

Executed on 2026-09-09:

- `make test`: PASS. Multiboot, ELF64/x86-64, sections, and core GUI symbols.
- `python3 tools/test_qemu_gui.py`: PASS. Captured 1024x768 desktop frames for
  WhiteSur, MacTahoe, and Vector icon themes.
- `git diff --check`: PASS.
- `readelf` and `nm` inspection: PASS for ELF class, machine, entry point,
  aligned sections, core symbols, and newly added theme/font/scale symbols.

The test harness does not simulate every mouse gesture, window resize, or
filesystem persistence. No 60 FPS measurement facility exists, so 60 FPS is a
target rather than a measured result.
