# RatanaOS 64-bit Architecture & Technical Specification

## 1. Boot & Execution Flow
1. **Multiboot Entry**: The bootloader loads the kernel at `0x100000` (1MB) in 32-bit protected mode.
2. **4-Level Paging Initialization**:
   - `pml4_table`, `pdpt_table`, and `pd_table` are aligned to 4KB page boundaries in `.bss`.
   - `PML4[0]` points to `PDPT`.
   - `PDPT[0]` points to `PD`.
   - `PD` maps 512 entries with 2MB huge pages (`flag = 0x83`), identity-mapping the entire first 1GB of physical RAM.
3. **Control Registers & MSR Setup**:
   - `CR3` loaded with `pml4_table` base address.
   - `CR4.PAE` (bit 5), `CR4.OSFXSR` (bit 9), `CR4.OSXMMEXCPT` (bit 10) enabled.
   - `IA32_EFER` MSR (`0xC0000080`) bit 8 (`LME`) set.
   - `CR0.PG` (bit 31) and `CR0.PE` (bit 0) enabled.
4. **Long Mode Jump**:
   - 64-bit GDT loaded with `lgdt [gdt64_ptr]`.
   - Far jump `jmp 0x08:long_mode_entry` enters native 64-bit Long Mode.
   - 64-bit stack (`RSP`) aligned to 16 bytes.
   - Multiboot parameters passed in `RDI` and `RSI` via SysV x86_64 ABI to `kernel_main`.

## 2. Memory Map
- `0x00000000 - 0x000FFFFF` : BIOS IVT, BDA, VGA Text Buffer (`0xB8000`).
- `0x00100000 - 0x003FFFFF` : Kernel `.text`, `.rodata`, `.data`, `.bss`, Page Tables, and 64 KiB Stack.
- `0x00400000 - 0x01400000` : 16 MB Dynamic Kernel Heap (`kmalloc`, `kfree`).
- `0x01400000 - 0x10000000` : 256 MB Physical Memory Managed by PMM Bitmap.
- `0xFD000000 / 0xE0000000` : VBE 32-bit Linear Framebuffer (1024x768x32 bpp).

## 3. Interrupts & Exceptions
- 256-entry 16-byte IDT gates.
- Dual 8259 PIC remapped to IRQ vectors `32-47`.
- Assembly stubs in `interrupts.asm` save all 64-bit registers (`RAX` through `R15`), call C dispatchers with `RDI = registers_t*`, and return with `iretq`.

## 4. Graphics & Compositor
- Double-buffered 32-bit linear framebuffer rendering at 60 FPS.
- Alpha blending compositing for translucent floating Dock and window drop shadows.
- Precomputed offscreen wallpaper gradient buffer for high-framerate tear-free animation.
