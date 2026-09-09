# RatanaOS — Arch Linux Build and Setup Guide

## 1. Overview
This guide provides the official instructions for building, testing, and debugging **RatanaOS x86_64** on **Arch Linux** and Arch-based distributions (EndeavourOS, Garuda Linux, Manjaro).

---

## 2. Arch Linux Dependencies

RatanaOS requires a standard freestanding x86_64 toolchain, the NASM assembler, GRUB boot utilities, xorriso for ISO creation, and QEMU for execution.

Install all required packages via `pacman`:

```bash
sudo pacman -Syu
sudo pacman -S --needed \
    base-devel \
    git \
    gcc \
    binutils \
    make \
    nasm \
    grub \
    xorriso \
    mtools \
    dosfstools \
    qemu-desktop \
    gdb \
    python
```

### Verifying Toolchain Versions
Run the following verification command to verify your environment:

```bash
gcc --version
ld --version
make --version
nasm --version
grub-mkrescue --version
xorriso --version
qemu-system-x86_64 --version
gdb --version
```

---

## 3. Toolchain & Compiler Requirements

RatanaOS is compiled as a 64-bit freestanding OS (`-ffreestanding`).

### Compiler Configuration
- **Native Host Toolchain**: The project is designed to build with standard Arch Linux `gcc` and `binutils` using the following architectural flags:
  ```makefile
  CFLAGS = -m64 -mgeneral-regs-only -ffreestanding -O2 -Wall -Wextra \
           -fno-pie -fno-stack-protector -fno-builtin -mno-red-zone \
           -mcmodel=small -nostdlib -nodefaultlibs
  ASFLAGS = -f elf64
  LDFLAGS = -m elf_x86_64 -T src/boot/linker.ld -nostdlib
  ```
- **Cross-Compiler Support (`x86_64-elf-gcc`)**:
  If an `x86_64-elf-*` cross-toolchain is installed (e.g. from the Arch User Repository `aur/x86_64-elf-gcc`), you can optionally override `CC`, `AS`, and `LD`:
  ```bash
  make CC=x86_64-elf-gcc LD=x86_64-elf-ld
  ```

---

## 4. Build Commands

Always run build commands from the root of the repository:

### Check Dependencies
```bash
make check-deps
```

### Build Kernel Binaries
```bash
make clean
make -j$(nproc)
```
Outputs:
- `build/ratanaos.bin` (Native 64-bit ELF kernel image)
- `build/ratanaos32.bin` (32-bit Multiboot conversion for legacy loaders)
- `build/*.elf` (27 freestanding userland binaries)

### Build Bootable ISO
```bash
make iso
```
Output:
- `build/ratanaos.iso` (Hybrid bootable ISO with GRUB Multiboot2/BIOS boot)

### Run Unit Tests
```bash
make test
```
Validates:
- Multiboot header alignment and checksum compliance
- 64-bit ELF section layouts and entry point (`0x100010`)
- Exported kernel, GUI, and icon symbols

---

## 5. QEMU Testing

### Graphical Desktop Mode
```bash
qemu-system-x86_64 -cdrom build/ratanaos.iso -hda disk.img -m 512M -vga std
```

### Headless / Serial Console Mode
```bash
qemu-system-x86_64 -cdrom build/ratanaos.iso -hda disk.img -m 512M -serial stdio -display none
```

### Networking Emulation (Intel e1000 NIC)
```bash
qemu-system-x86_64 -cdrom build/ratanaos.iso -hda disk.img -m 512M -vga std \
    -net nic,model=e1000 -net user
```

---

## 6. GDB Debugging

To debug kernel startup or panic states using GDB:

1. **Launch QEMU with GDB stub frozen at boot**:
   ```bash
   qemu-system-x86_64 -cdrom build/ratanaos.iso -hda disk.img -m 512M -S -s
   ```
2. **In a separate terminal, attach GDB**:
   ```bash
   gdb build/ratanaos.bin
   (gdb) target remote localhost:1234
   (gdb) break kernel_main
   (gdb) continue
   ```

---

## 7. Common Arch Linux Issues & Troubleshooting

| Problem | Cause | Solution |
| :--- | :--- | :--- |
| `grub-mkrescue: command not found` | GRUB packages missing | Install `grub`, `xorriso`, `mtools`, `dosfstools` via `pacman -S`. |
| `relocation truncated to fit: R_X86_64_32` | Missing `-mcmodel=small` or `-fno-pie` | Ensure `CFLAGS` contains `-mcmodel=small -fno-pie`. |
| `undefined reference to '__stack_chk_fail'` | GCC stack protector enabled | Add `-fno-stack-protector` to `CFLAGS`. |
| `qemu-system-x86_64: command not found` | QEMU not installed | Install `qemu-desktop` or `qemu-system-x86` via `pacman -S`. |
