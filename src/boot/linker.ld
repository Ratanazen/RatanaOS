/* Linker script for RatanaOS 32-bit kernel */
ENTRY(_start)

SECTIONS
{
    /* The kernel will live at 1MB (0x100000) */
    . = 1M;

    /* First put the multiboot header, as it is required to be within the first 8KiB */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data. */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata*)
    }

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }

    /* The compiler may produce other sections, put them in the proper place in
       in this file, if you'd like to include them in the binary */
}
