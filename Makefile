CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -fno-builtin -nostdlib -nodefaultlibs
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T src/boot/linker.ld -nostdlib

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
         $(BUILD_DIR)/shell.o \
         $(BUILD_DIR)/matrix.o \
         $(BUILD_DIR)/snake.o \
         $(BUILD_DIR)/vga.o \
         $(BUILD_DIR)/keyboard.o \
         $(BUILD_DIR)/rtc.o \
         $(BUILD_DIR)/serial.o \
         $(BUILD_DIR)/speaker.o \
         $(BUILD_DIR)/pci.o \
         $(BUILD_DIR)/string.o \
         $(BUILD_DIR)/stdio.o

OBJS = $(ASM_OBJS) $(C_OBJS)
TARGET = $(BUILD_DIR)/ratanaos.bin
ISO_TARGET = $(BUILD_DIR)/ratanaos.iso

.PHONY: all clean run run-iso iso dirs

all: dirs $(TARGET)

dirs:
	@mkdir -p $(BUILD_DIR)

# ASM Objects
$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot/boot.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/gdt_flush.o: $(SRC_DIR)/kernel/gdt_flush.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/interrupts.o: $(SRC_DIR)/kernel/interrupts.asm
	$(AS) $(ASFLAGS) $< -o $@

# Kernel C Objects
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

$(BUILD_DIR)/shell.o: $(SRC_DIR)/kernel/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/matrix.o: $(SRC_DIR)/kernel/matrix.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/snake.o: $(SRC_DIR)/kernel/snake.c
	$(CC) $(CFLAGS) -c $< -o $@

# Driver C Objects
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

# Library C Objects
$(BUILD_DIR)/string.o: $(SRC_DIR)/lib/string.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/stdio.o: $(SRC_DIR)/lib/stdio.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link Kernel Binary
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@echo "\n>>> Successfully built RatanaOS 2026 Kernel: $(TARGET) <<<\n"

# Create Bootable ISO
iso: $(TARGET)
	@mkdir -p iso/boot
	@cp $(TARGET) iso/boot/ratanaos.bin
	grub-mkrescue -o $(ISO_TARGET) iso
	@echo "\n>>> Successfully built Bootable ISO: $(ISO_TARGET) <<<\n"

# Run in QEMU with direct kernel boot & COM1 serial redirect to stdio
run: $(TARGET)
	qemu-system-x86_64 -kernel $(TARGET) -serial stdio

# Run ISO in QEMU
run-iso: iso
	qemu-system-x86_64 -cdrom $(ISO_TARGET) -serial stdio

# Clean
clean:
	rm -rf $(BUILD_DIR)/* iso/boot/ratanaos.bin
