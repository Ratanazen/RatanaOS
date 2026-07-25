# RatanaOS Live Build Makefile

EDITION ?= standard
VERSION ?= 21.0.0

.PHONY: all config build build-all clean rebuild iso verify vm release usb-build usb-list usb-flash usb-run live-build-source

all: iso verify

live-build-source:
	@if [ ! -d "builder/live-build" ]; then \
		echo "==> Cloning Debian live-build from Salsa..."; \
		git clone https://salsa.debian.org/live-team/live-build.git builder/live-build; \
	fi

config: live-build-source
	@echo "==> Configuration is now handled natively within the build scripts."

build: live-build-source
	@echo "==> Building RatanaOS (${EDITION} edition) using official live-build..."
	@chmod +x builder/build-ratanaos.sh
	@sudo bash builder/build-ratanaos.sh ${EDITION}

build-all: live-build-source
	@echo "==> Building ALL RatanaOS editions using official live-build..."
	@chmod +x builder/build-ratanaos.sh
	@for edition in lite standard developer cyber; do \
		echo ""; \
		echo "▶ Building edition: $$edition..."; \
		sudo bash builder/build-ratanaos.sh $$edition; \
	done

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
	@./scripts/test-qemu.sh $(EDITION)

release: iso verify
	@echo "==> Generating Release Checksums..."
	@cd releases && sha256sum RatanaOS-*.iso > SHA256SUMS
	@echo "==> Release complete!"

usb-build:
	@echo "==> Building RatanaOS Live USB ISO..."
	@chmod +x builder/build-live-usb.sh
	@./builder/build-live-usb.sh

usb-list:
	@chmod +x scripts/run-usb.sh
	@./scripts/run-usb.sh list

usb-flash:
	@chmod +x scripts/run-usb.sh
	@./scripts/run-usb.sh flash ${DRIVE} ${EDITION}

usb-run:
	@chmod +x scripts/run-usb.sh
	@./scripts/run-usb.sh boot ${DRIVE}
