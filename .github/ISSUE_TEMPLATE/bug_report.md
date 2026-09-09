---
name: Bug Report
about: Report a bug in RatanaOS
title: '[BUG] '
labels: 'bug'
---

## Environment
- **RatanaOS Variant**: desktop / minimal / server
- **Boot Mode**: BIOS / UEFI
- **Hardware**: Physical machine / QEMU / VirtualBox / VMware
- **ISO Version / Date**: (e.g., v2.0.1, 2026-09-09)

## Description
A clear description of the bug.

## Steps to Reproduce
1. ...
2. ...

## Expected Behavior
What you expected to happen.

## Actual Behavior
What actually happened.

## Relevant Logs
Attach relevant log files:
- Plymouth: `journalctl -b -u plymouth`
- Xorg: `cat /var/log/Xorg.0.log`
- System: `journalctl -b --no-pager | tail -100`
- Calamares: `cat /var/log/installer/calamares.log`

## Screenshots
If applicable, add screenshots.
