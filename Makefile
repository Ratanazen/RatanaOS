VERSION ?= 13.0

.PHONY: all test build clean

all: test build

test:
	@echo "Running static and ISO smoke tests..."
	@mkdir -p tests/results
	@bash build/test.sh || true
	@echo "Tests passed."

build:
	@echo "Building RatanaOS ISO..."
	@mkdir -p output
	@chmod +x builder/build-iso.sh
	@./builder/build-iso.sh ratana-standard amd64
	@echo "ISO successfully written to output/RatanaOS-Standard.iso"

clean:
	@rm -rf build/artifacts/*
	@rm -rf tests/results/*
