/* Linker script for RatanaOS 64-bit x86_64 kernel */
ENTRY(_start)

SECTIONS
{
    /* The kernel starts at 1MB (0x100000) */
    . = 1M;

    /* Multiboot header must be within the first 8KiB */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata*)
    }

    /* Initialized read-write data */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Uninitialized data and stack / page tables */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
