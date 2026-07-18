# RatanaOS Live Build Makefile

EDITION ?= standard
VERSION ?= 15.0

.PHONY: all config build clean rebuild iso verify vm release

all: iso verify

config:
	@echo "==> Configuring live-build for RatanaOS (${EDITION})..."
	@chmod +x builder/config.sh
	@./builder/config.sh ${EDITION}

build:
	@echo "==> Building RatanaOS..."
	@chmod +x builder/build.sh
	@./builder/build.sh ${EDITION}

clean:
	@echo "==> Cleaning live-build workspace..."
	lb clean || true
	rm -rf output/*

rebuild: clean config build

iso: config build

verify:
	@echo "==> Verifying ISO..."
	@chmod +x scripts/verify-iso.sh
	@./scripts/verify-iso.sh ${EDITION}

vm:
	@echo "==> Running VM Test..."
	@chmod +x scripts/test-qemu.sh
	@./scripts/test-qemu.sh ${EDITION}

release: iso verify
	@echo "==> Generating Release Checksums..."
	@cd output && sha256sum RatanaOS-*.iso > SHA256SUMS
	@echo "==> Release complete!"
