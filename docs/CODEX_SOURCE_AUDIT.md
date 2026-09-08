# RatanaOS Source Audit

Audit date: 2026-09-09. This report is based on a read-only inspection of the
repository before GUI feature changes.

## Repository structure

RatanaOS is a freestanding C and NASM x86-64 kernel. `src/boot` contains the
Multiboot/long-mode entry and linker script; `src/kernel` contains kernel
services and the desktop; `src/drivers` contains hardware and framebuffer
drivers; `src/lib` supplies freestanding string/stdio routines. `build` holds
generated artifacts. `iso/boot/grub/grub.cfg` is the GRUB boot configuration.
`tools/bake_icons.py` generates the checked-in icon asset table.

## Kernel architecture

`kernel_main` in `src/kernel/kernel.c` initializes serial and VGA, GDT, IDT,
PIC, PIT, PMM, heap, RTC, CPUID/PCI, keyboard, then `gui_init`. Interrupts are
enabled before `gui_start`; leaving the GUI returns to the VGA shell. The
kernel is native 64-bit after the assembly transition in `src/boot/boot.asm`.

## Graphics and framebuffer architecture

`src/drivers/gfx.c` owns a `gfx_context_t` with front, back, and wallpaper
buffers. It reads Multiboot framebuffer width, height, pitch, bpp, and address;
when unavailable it configures a Bochs/VBE 1024x768x32 linear framebuffer.
Rendering is software composited to a heap backbuffer and copied by
`gfx_swap_buffers` once per GUI frame. Pixels are stored as `0x00RRGGBB` in
the renderer; icon assets are ARGB with alpha in the high byte.

The implementation has clipping primitives and alpha blending. However, the
backbuffer is tightly packed and several frontbuffer operations index/copy by
`width * 4`, not framebuffer pitch. This is safe for the current 1024x768x32
configuration but needs correction for arbitrary pitched framebuffers. Several
shape functions also require early validation for non-positive dimensions and
overflow-safe clipping.

## Font architecture

`src/kernel/font.c` scales the embedded `font8x16_basic` bitmap font to four
fixed sizes: Small (6x12), Regular (8x16), Large (12x24), and Title (16x32).
It exposes measuring, clipped drawing, centered drawing, and an active size.
There is no FreeType dependency, glyph cache, font family selection, or
separate role abstraction today.

## GUI and window architecture

`src/kernel/gui.c` is the retained desktop controller. It owns a static array
of up to 16 `window_t` values and registers 12 built-in windows including
Finder, Terminal, System Settings, and About. Existing windows support open,
close, focus/z-order, drag, minimize, and a two-size zoom toggle. Chrome is
theme-driven with rounded frames, shadows, title bars, and traffic lights.
There is no public window-manager API, general resize, fullscreen state, or
widget event routing layer; click handlers are per-window callbacks.

## Dock, desktop, Finder, and menu bar

`src/kernel/dock.c` draws a bottom dock with settings-controlled icon size,
spacing, and hover magnification. It uses the icon APIs, so its appearance is
independent of the UI theme. `gui.c` also draws a desktop with three clickable
icons and Finder as a static, illustrative content view. `src/kernel/menubar.c`
draws the top bar and Apple menu; it uses current theme colors except for a few
legacy literals.

## Icon architecture

`src/drivers/icons.c` preserves the public `icon_set_theme`, `icon_theme_next`,
`icon_get_theme_name`, and `gfx_draw_icon_rgba` APIs. The enum in
`src/include/icons_assets.h` has `ICON_THEME_WHITESUR`, `ICON_THEME_MACTAHOE`,
and `ICON_THEME_VECTOR`; vector is the procedural fallback. Generated assets
are in `src/drivers/icons_assets.c`. Their pre-change SHA-256 is
`0f6ab6f0c2de1142bafc11d144e423579b961b18fffd3e9ce5b3e0c0d10a42e7`.
They are out of scope for modification.

## Settings, input, and shell

`src/kernel/settings.c` already centralizes UI theme, icon theme, accent,
scale, font size, dock sizing, transparency, shadows, and window radius. Its
save/load backend is an in-memory checksum-protected copy, not persistent
storage; documentation and messages must not claim disk persistence.

PS/2 keyboard IRQ1 places characters in a ring buffer and tracks modifiers.
PS/2 mouse IRQ12 decodes three-byte packets, clamps coordinates, and exposes
button state. `gui.c` polls these states once per frame. Input currently has no
typed mouse wheel/key-up event queue. `src/kernel/shell.c` already supports
theme, icon, scale, font, dock, and settings commands.

## Build and linker architecture

The Makefile compiles freestanding `-m64` C and ELF64 NASM objects, links with
`ld -m elf_x86_64 -T src/boot/linker.ld`, and creates `build/ratanaos.bin`.
It additionally creates `ratanaos32.bin` for QEMU's direct kernel loader.
`make test` validates the Multiboot header, ELF metadata, sections, and key
symbols. The linker places the Multiboot header in `.text` at 1 MiB and aligns
major sections to 4 KiB. GRUB boots the ELF64 kernel via `multiboot`.

## Planned change boundaries

Files likely to modify:

- `src/drivers/gfx.c`, `src/include/gfx.h` for pitch-aware safe drawing.
- `src/kernel/theme.c`, `src/include/theme.h` for aliases, tokens, and
  centralized visual policy.
- `src/kernel/font.c`, `src/include/font.h` for role and size abstraction.
- `src/kernel/settings.c`, `src/include/settings.h`, `src/kernel/shell.c` for
  truthful persistence abstraction and configuration commands.
- `src/kernel/gui.c`, `src/include/gui.h`, `src/kernel/dock.c`,
  `src/kernel/menubar.c`, and controls only where a reusable, tested path can
  replace direct GUI literals.
- `Makefile` only for new source files; `README.md` and `docs/` for accurate
  documentation and test records.

Files to create:

- Documentation required by the GUI work, plus small focused headers/sources
  for design tokens or wallpaper only if they reduce duplication.

Files to leave untouched:

- `src/boot/boot.asm`, `src/boot/linker.ld`, `iso/boot/grub/grub.cfg`.
- `src/drivers/icons_assets.c`, `src/include/icons_assets.h`, and
  `tools/bake_icons.py`.
- Existing GDT/IDT, paging, ISR, PMM, heap, keyboard, and mouse driver code
  unless a tested integration requires a narrowly scoped change.

## Implementation plan

1. Harden framebuffer clipping, dimensions, pitch-aware swaps, and icon
   bounds without changing pixel semantics.
2. Extend the existing theme/font/scale systems rather than duplicate them;
   add semantic design tokens and compatibility APIs.
3. Add a wallpaper and settings-backend abstraction, documenting that the
   current backend is volatile.
4. Improve the existing window controller and controls incrementally, using
   the theme and scale APIs; preserve the static no-allocation render loop.
5. Complete shell/shortcut integration, update settings UI where practical,
   then build and run automated ELF/Multiboot verification after each group.
6. Review the diff, re-hash untouched icon assets, and record only test results
   that were actually run.
