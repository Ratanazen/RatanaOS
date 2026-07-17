SHELL := /bin/bash

.PHONY: help configure build clean test

help:
	@echo "RatanaOS Phase 1"
	@echo "  make configure  Generate live-build configuration"
	@echo "  make build      Build the amd64 live/install ISO"
	@echo "  make clean      Remove generated build state"
	@echo "  make test       Run static project checks"

configure:
	./scripts/configure-build.sh

build:
	./scripts/build-iso.sh

clean:
	./scripts/clean.sh

test:
	./tests/run.sh

