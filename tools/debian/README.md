# 🐧 RatanaOS Debian Live-Build Environment

This directory contains the Debian tooling and configuration used to build a clean **Debian GNU/Linux 13 "Trixie" (amd64)** root filesystem and live image for RatanaOS integration.

---

## 📁 Structure

```text
tools/debian/
├── auto/
│   └── config                  # live-build automation profile (Trixie amd64)
├── config/
│   └── package-lists/
│       └── minimal.list.chroot # Minimal package manifest (busybox, coreutils, libc6, bash)
└── README.md                   # Toolchain documentation
```

---

## 🛠️ Build Pipeline

The Debian userspace build process is managed by `tools/ratana-debian-build.sh`:

1. **Verify Tooling**: Probes for `live-build` (`lb`) or `debootstrap`.
2. **Build RootFS**: Generates minimal Debian 13 "Trixie" userspace files.
3. **Package for RatanaOS**: Passes the filesystem tree to `tools/bake_debian_rootfs.py`.
4. **VFS Integration**: Compiles into `build/debian.img` and `src/kernel/fs/debian_data.c`, mounted at `/system/debian`.
