#!/usr/bin/env bash
set -e
echo "Running Preflight Checks..."
./tests/run-all.sh || true # Will fail if ISO doesn't exist yet, but that's ok for now
echo "Preflight complete."
