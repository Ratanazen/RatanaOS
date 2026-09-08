# Build

Use `make test` for the supported baseline build and binary checks. It compiles
freestanding x86-64 C/NASM, links `build/ratanaos.bin` as ELF64, creates the
ELF32 direct-QEMU container, checks Multiboot, and inspects essential symbols.

`make iso` builds a GRUB ISO when `grub-mkrescue` and xorriso are installed.
The linker remains `src/boot/linker.ld`; no host libraries are linked.
