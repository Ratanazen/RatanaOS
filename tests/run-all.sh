#!/usr/bin/env bash
set -e
echo "Running all RiOS tests..."
for test_script in tests/test-*.sh; do
    if [ -x "$test_script" ]; then
        echo "=============================================="
        echo "Running $test_script"
        echo "=============================================="
        "$test_script" || { echo "Test $test_script FAILED"; exit 1; }
    fi
done
echo "All static tests passed."
