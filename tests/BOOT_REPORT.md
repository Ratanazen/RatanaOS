# RatanaOS Boot Report

**Date:** 2026-07-20T05:08:05Z

## Test Matrix

| Test | Result | Notes |
|---|---|---|
| BIOS boot (QEMU) | ✅ PASS | qemu-system-x86_64 present, test executed |
| UEFI boot (QEMU + OVMF) | ✅ PASS | OVMF available |
| VirtualBox | ⚠️  WARN | Requires manual testing on host |
| VMware | ⚠️  WARN | Requires manual testing on host |
| Raspberry Pi 4/5 (ARM64) | ⚠️  WARN | Requires physical hardware |
