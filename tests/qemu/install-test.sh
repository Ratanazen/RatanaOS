#!/usr/bin/env bash
set -e
echo "Running QEMU End-to-End Installation Test..."
python3 tests/qemu/install-test.py
echo "QEMU Test Complete."
