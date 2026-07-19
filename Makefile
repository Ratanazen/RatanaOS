# RatanaOS Live Build Makefile

EDITION ?= standard
VERSION ?= 18.0

.PHONY: all config build clean rebuild iso verify vm release

all: iso verify

config:
	@echo "==> Configuration is now handled natively within the build scripts."

build:
	@echo "==> Building RatanaOS..."
	@if [ "${EDITION}" = "live" ]; then \
		chmod +x builder/build-live-usb.sh; \
		./builder/build-live-usb.sh; \
	else \
		chmod +x builder/build-iso.sh; \
		./builder/build-iso.sh ${EDITION}; \
	fi

clean:
	@echo "==> Cleaning live-build workspace..."
	lb clean || true
	rm -rf releases/*

rebuild: clean build

iso: build

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
	@cd releases && sha256sum RatanaOS-*.iso > SHA256SUMS
	@echo "==> Release complete!"
