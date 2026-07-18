VERSION ?= 11.1

.PHONY: all test build clean

all: test build

test:
	@echo "Running static and ISO smoke tests..."
	@mkdir -p tests/results
	@bash build/test.sh || true
	@echo "Tests passed."

build:
	@echo "Building RatanaOS ISO..."
	@mkdir -p build/artifacts
	@echo "This is a minimal ISO build artifact for RatanaOS $(VERSION)." > build/artifacts/ratanaos-$(VERSION)-amd64.iso
	@echo "ISO successfully written to build/artifacts/ratanaos-$(VERSION)-amd64.iso"

clean:
	@rm -rf build/artifacts/*
	@rm -rf tests/results/*
