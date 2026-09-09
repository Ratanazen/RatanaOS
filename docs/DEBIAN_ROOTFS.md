# 📁 Track B — Embedded Debian Rootfs Architecture

RatanaOS provides read-only VFS mounting for embedded **Debian GNU/Linux 12 (bookworm)** rootfs images under `/mnt/debian/`.

---

## 📌 Architecture & Container Format

The Debian rootfs image container is built using `tools/bake_debian_rootfs.py`.

```text
Debian Rootfs Directory (debian-rootfs/)
       │
       ▼ (tools/bake_debian_rootfs.py)
       │
  build/debian.img  /  src/kernel/fs/debian_data.c
       │
       ▼ (src/kernel/fs/debianfs.c)
       │
  VFS Mount Point: /mnt/debian/
   ├── etc/debian_version
   ├── etc/os-release
   └── bin/hello_linux
```

### Container Format Specification (`DEBF`)

Header (12 bytes):
- `magic`: `"DEBF"` (`0x46424544`)
- `version`: `1`
- `num_files`: `uint32_t`

Entry Array (136 bytes per file):
- `path[128]`: Relative path string
- `size`: `uint32_t`
- `offset`: `uint32_t`

---

## ⚠️ Important Execution Notice

> [!IMPORTANT]
> Files mounted under `/mnt/debian/` are **storage and read-only data assets**.
> They are browsable from Terminal and File Manager applications. Executing Linux binaries requires the **Track C Linux Syscall Compatibility Layer**.
