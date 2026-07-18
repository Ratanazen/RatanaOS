# ISO Structure Verification

The generated `output/RatanaOS-Standard.iso` contains the exact required bootable structure:

```
/
├── boot/
│   └── grub/
│       ├── i386-pc/
│       ├── x86_64-efi/
│       ├── grub.cfg
│       └── fonts/
├── EFI/
│   └── BOOT/
├── live/
│   ├── vmlinuz
│   ├── initrd.img
│   └── filesystem.squashfs
├── vmlinuz
└── initrd
```

- **Boot Catalog**: El Torito present (via `grub-mkrescue`)
- **BIOS Boot**: `i386-pc` boot image present
- **UEFI Boot**: `x86_64-efi` boot image present
- **Filesystem**: `filesystem.squashfs` accurately mapped to `/live` directory
