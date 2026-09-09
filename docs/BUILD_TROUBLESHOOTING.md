# RatanaOS — Build Troubleshooting Guide

This guide covers common compilation, linking, ISO generation, and virtualization issues encountered when building **RatanaOS** on Linux host systems.

---

## 1. Toolchain & Compilation Errors

### `error: SSE/AVX register return with SSE disabled`
- **Cause**: Using floating-point types (`float`, `double`) or passing vector registers inside kernel code compiled with `-mgeneral-regs-only`.
- **Solution**: The kernel operates exclusively with integer arithmetic. Replace floating-point logic with scaled integer math (e.g. integer percentages, millisecond integers).

### `error: undefined reference to '__stack_chk_fail'`
- **Cause**: The compiler inserted stack canary checks because `-fstack-protector` was inadvertently enabled.
- **Solution**: Verify that `-fno-stack-protector` is specified in `CFLAGS` in the `Makefile`.

### `relocation truncated to fit: R_X86_64_32S`
- **Cause**: Linker attempting to resolve a 64-bit absolute address in a 32-bit immediate field without the correct code model.
- **Solution**: Ensure `-mcmodel=small` and `-fno-pie` are passed to GCC.

---

## 2. Linker & ELF Layout Issues

### `warning: build/boot.o: section '.multiboot' not found within 8KiB`
- **Cause**: The Multiboot header section `.multiboot` was placed after large text sections.
- **Solution**: Inspect `src/boot/linker.ld`. Ensure `*(.multiboot)` is positioned immediately at the start of the `.text` segment:
  ```ld
  .text BLOCK(4K) : ALIGN(4K)
  {
      *(.multiboot)
      *(.text)
  }
  ```

### `objcopy: cannot recognize binary format`
- **Cause**: Host `objcopy` version mismatch or incorrect target architecture flag.
- **Solution**: Ensure `objcopy -I elf64-x86-64 -O elf32-i386` is used for creating the 32-bit multiboot trampoline kernel.

---

## 3. ISO Generation Issues

### `grub-mkrescue: xorriso not found`
- **Cause**: `xorriso` package is missing on the host.
- **Solution**:
  - Arch Linux: `sudo pacman -S xorriso mtools dosfstools`
  - Debian/Ubuntu: `sudo apt-get install xorriso mtools dosfstools`

### `xorriso: FAILURE : ... cannot find system area image`
- **Cause**: GRUB i386-pc boot assets missing.
- **Solution**: Install the `grub-pc-bin` or `grub` package for BIOS/multiboot image support.

---

## 4. QEMU Runtime & Display Issues

### Black Screen or No Graphics
- **Cause**: QEMU started without a graphical display driver that supports VESA/VBE modes.
- **Solution**: Run QEMU with `-vga std` (standard VESA BIOS Extensions):
  ```bash
  qemu-system-x86_64 -cdrom build/ratanaos.iso -m 512M -vga std
  ```

### Mouse Cursor Out of Sync
- **Cause**: Absolute tablet pointing device not configured.
- **Solution**: Add `-usb -device usb-tablet` to the QEMU command line.

### Disk Warning: `Image format was not specified for 'disk.img'`
- **Cause**: QEMU defaults to probing raw images.
- **Solution**: Specify raw format explicitly:
  ```bash
  qemu-system-x86_64 -drive file=disk.img,format=raw,index=0,media=disk
  ```
