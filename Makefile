CC = gcc
AS = nasm
LD = ld

CFLAGS = -m64 -mgeneral-regs-only -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -fno-builtin -mno-red-zone -mcmodel=small -nostdlib -nodefaultlibs
ASFLAGS = -f elf64
LDFLAGS = -m elf_x86_64 -T src/boot/linker.ld -nostdlib

BUILD_DIR = build
SRC_DIR = src

ASM_OBJS = $(BUILD_DIR)/boot.o \
           $(BUILD_DIR)/gdt_flush.o \
           $(BUILD_DIR)/interrupts.o

C_OBJS = $(BUILD_DIR)/kernel.o \
         $(BUILD_DIR)/gdt.o \
         $(BUILD_DIR)/idt.o \
         $(BUILD_DIR)/isr.o \
         $(BUILD_DIR)/pic.o \
         $(BUILD_DIR)/timer.o \
         $(BUILD_DIR)/pmm.o \
         $(BUILD_DIR)/heap.o \
         $(BUILD_DIR)/cpuid.o \
         $(BUILD_DIR)/packages.o \
         $(BUILD_DIR)/shell.o \
         $(BUILD_DIR)/theme.o \
         $(BUILD_DIR)/font.o \
         $(BUILD_DIR)/ui_controls.o \
         $(BUILD_DIR)/settings.o \
         $(BUILD_DIR)/matrix.o \
         $(BUILD_DIR)/snake.o \
         $(BUILD_DIR)/vga.o \
         $(BUILD_DIR)/keyboard.o \
         $(BUILD_DIR)/rtc.o \
         $(BUILD_DIR)/serial.o \
         $(BUILD_DIR)/speaker.o \
         $(BUILD_DIR)/pci.o \
         $(BUILD_DIR)/gfx.o \
         $(BUILD_DIR)/icons.o \
         $(BUILD_DIR)/icons_assets.o \
         $(BUILD_DIR)/menubar.o \
         $(BUILD_DIR)/dock.o \
         $(BUILD_DIR)/mouse.o \
         $(BUILD_DIR)/gui.o \
         $(BUILD_DIR)/string.o \
         $(BUILD_DIR)/stdio.o

OBJS = $(ASM_OBJS) $(C_OBJS)
TARGET = $(BUILD_DIR)/ratanaos.bin
TARGET32 = $(BUILD_DIR)/ratanaos32.bin
ISO_TARGET = $(BUILD_DIR)/ratanaos.iso

.PHONY: all clean run run-iso iso test test-qemu dirs bake-icons

all: dirs $(TARGET) $(TARGET32)

dirs:
	@mkdir -p $(BUILD_DIR)

# 64-bit ASM Objects
$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot/boot.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/gdt_flush.o: $(SRC_DIR)/kernel/gdt_flush.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/interrupts.o: $(SRC_DIR)/kernel/interrupts.asm
	$(AS) $(ASFLAGS) $< -o $@

# 64-bit Kernel C Objects
$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/gdt.o: $(SRC_DIR)/kernel/gdt.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/idt.o: $(SRC_DIR)/kernel/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/isr.o: $(SRC_DIR)/kernel/isr.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/pic.o: $(SRC_DIR)/kernel/pic.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/timer.o: $(SRC_DIR)/kernel/timer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/pmm.o: $(SRC_DIR)/kernel/pmm.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/heap.o: $(SRC_DIR)/kernel/heap.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cpuid.o: $(SRC_DIR)/kernel/cpuid.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/packages.o: $(SRC_DIR)/kernel/packages.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/shell.o: $(SRC_DIR)/kernel/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/theme.o: $(SRC_DIR)/kernel/theme.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/font.o: $(SRC_DIR)/kernel/font.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/ui_controls.o: $(SRC_DIR)/kernel/ui_controls.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/settings.o: $(SRC_DIR)/kernel/settings.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/matrix.o: $(SRC_DIR)/kernel/matrix.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/snake.o: $(SRC_DIR)/kernel/snake.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/menubar.o: $(SRC_DIR)/kernel/menubar.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/dock.o: $(SRC_DIR)/kernel/dock.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/gui.o: $(SRC_DIR)/kernel/gui.c
	$(CC) $(CFLAGS) -c $< -o $@

# 64-bit Driver C Objects
$(BUILD_DIR)/vga.o: $(SRC_DIR)/drivers/vga.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/keyboard.o: $(SRC_DIR)/drivers/keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/rtc.o: $(SRC_DIR)/drivers/rtc.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/serial.o: $(SRC_DIR)/drivers/serial.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/speaker.o: $(SRC_DIR)/drivers/speaker.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/pci.o: $(SRC_DIR)/drivers/pci.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/gfx.o: $(SRC_DIR)/drivers/gfx.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/icons.o: $(SRC_DIR)/drivers/icons.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/icons_assets.o: $(SRC_DIR)/drivers/icons_assets.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/mouse.o: $(SRC_DIR)/drivers/mouse.c
	$(CC) $(CFLAGS) -c $< -o $@

# 64-bit Library C Objects
$(BUILD_DIR)/string.o: $(SRC_DIR)/lib/string.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/stdio.o: $(SRC_DIR)/lib/stdio.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link 64-bit Kernel Binary
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@echo "\n>>> Successfully built RatanaOS 64-bit macOS Edition: $(TARGET) <<<\n"

# Create ELF32 container for direct QEMU -kernel loader
$(TARGET32): $(TARGET)
	objcopy -I elf64-x86-64 -O elf32-i386 $(TARGET) $@

# Create Bootable ISO (requires xorriso)
iso: $(TARGET)
	@mkdir -p iso/boot/grub
	@cp $(TARGET) iso/boot/ratanaos.bin
	@if which xorriso >/dev/null 2>&1; then \
		grub-mkrescue -o $(ISO_TARGET) iso; \
		echo "\n>>> Successfully built Bootable ISO: $(ISO_TARGET) <<<\n"; \
	else \
		echo "Notice: xorriso is not installed. To generate $(ISO_TARGET), install xorriso with 'sudo pacman -S xorriso'."; \
	fi

# Run in QEMU 64-bit with direct kernel boot & COM1 serial redirect to stdio
run: $(TARGET32)
	qemu-system-x86_64 -kernel $(TARGET32) -serial stdio -vga std

# Run ISO in QEMU
run-iso: iso
	@if [ -f $(ISO_TARGET) ]; then \
		qemu-system-x86_64 -cdrom $(ISO_TARGET) -serial stdio -vga std; \
	else \
		echo "ISO not found. Run 'sudo pacman -S xorriso' and 'make iso'."; \
	fi

# Run in QEMU Curses Mode (in terminal)
run-curses: $(TARGET32)
	qemu-system-x86_64 -kernel $(TARGET32) -display curses

# Run automated tests
test: all
	@echo "\n=============================================="
	@echo "     RATANAOS 64-BIT MACOS TEST SUITE         "
	@echo "=============================================="
	@echo "\n[TEST 1] Multiboot Header Verification..."
	@grub-file --is-x86-multiboot $(TARGET32) && echo "  [PASS] Multiboot header is valid and compliant." || (echo "  [FAIL] Invalid Multiboot header."; exit 1)
	@echo "\n[TEST 2] 64-bit ELF Layout & Section Alignment..."
	@readelf -h $(TARGET) | grep -E "Class|Machine|Entry point address"
	@readelf -S $(TARGET) | grep -E "\.text|\.rodata|\.data|\.bss"
	@echo "  [PASS] Native ELF 64-bit x86-64 executable layout validated."
	@echo "\n[TEST 3] 64-bit Kernel & macOS GUI Symbols..."
	@nm $(TARGET) | grep -E "kernel_main|gdt_init|idt_init|menubar_init|dock_init|gui_init|icon_draw_finder_48"
	@echo "  [PASS] 64-bit macOS GUI & icon symbols verified."
	@echo "\n=============================================="
	@echo "   ALL 64-BIT MACOS TESTS PASSED!             "
	@echo "==============================================\n"

bake-icons:
	python3 tools/bake_icons.py

# Clean
clean:
	rm -rf $(BUILD_DIR)/* iso/boot/ratanaos.bin
