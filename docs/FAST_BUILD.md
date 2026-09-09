# ⚡ RatanaOS Fast Build Engine (< 10 Minutes)

## Overview
Standard Debian live-build uses single-threaded or CPU-intensive XZ compression (`-comp xz`) and re-downloads package lists, which can take 45–90 minutes on a cold build.

RatanaOS provides an accelerated build pipeline (`make iso-fast` / `tools/fast-live-build.sh`) designed to produce a **full bootable hybrid ISO in under 10 minutes** on rebuilds.

---

## Performance Optimizations

| Feature | Standard Build | Fast Build (`make iso-fast`) | Speedup |
| :--- | :--- | :--- | :---: |
| **Compression Engine** | `xz` (high CPU, slow) | `zstd -3 -T0` (multi-threaded Zstandard) | **~10x faster** |
| **Package Cache** | Ephemeral | Persistent local `.deb` archive cache | **Zero re-downloads** |
| **Debian Installer** | Full text/udeb installer download | Calamares-native (desktop installer only) | **Skips 300+ udebs** |
| **Build Target** | Monolithic pipeline | Staged, incremental container cache | **Cached stages** |

---

## Usage

### 🚀 Run Fast Build
```bash
make iso-fast
```
*or directly:*
```bash
./tools/fast-live-build.sh
```

### 🖥️ Run Live in VM Immediately After Build
```bash
make live-run
```

---

## Output Files
- **ISO Image**: `build/ratanaos-live-amd64.hybrid.iso`
- **SHA-256 Checksum**: `build/RatanaOS.iso.sha256`
- **Architecture**: x86_64 / amd64 (UEFI Secure Boot + BIOS Hybrid)
