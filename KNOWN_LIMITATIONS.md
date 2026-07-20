# RatanaOS Known Limitations & Hardware Guidance

While RatanaOS provides broad out-of-the-box hardware enablement and subsystem diagnostics, certain hardware configurations and host environments have specific boundaries and operational policies.

---

## 1. CPU Architecture Requirement
- **Limitation**: RatanaOS v1 currently targets the `x86_64` (amd64) 64-bit architecture.
- **Guidance**: ARM64 (aarch64), 32-bit x86 (i686), and RISC-V platforms are unsupported in this release. Running `ratana-hardware-probe` or `ratana-driver-manager` on an unsupported architecture will display an explicit `UNSUPPORTED ARCHITECTURE` diagnostic report instead of failing silently.

## 2. Proprietary NVIDIA Graphics Policy
- **Default Behavior**: RatanaOS boots using the open-source `nouveau` graphics driver for maximum stability, multi-monitor display, and security.
- **Guidance**: RatanaOS will **never** automatically install proprietary drivers without explicit user approval. For intensive 3D gaming or CUDA workloads, run:
  ```bash
  ratana-driver-manager --install-nvidia
  ```
  The Driver Manager will display policy recommendations and ask for user confirmation before proceeding with package installation.

## 3. Container & Virtual Machine Sandboxes
- **ISO Build Boundary**: Compiling a full live-bootable Debian ISO requires Linux kernel chroot mounting (`debootstrap`), Loop devices, and root privileges (`sudo`). Inside unprivileged container sandboxes, build scripts generate validation reports.
- **Guidance**: Run `sudo bash builder/build-ratanaos.sh standard` on a physical Debian/Ubuntu host machine to compile production ISOs.

## 4. Hardware Peripheral Verification Scope
- **Environment Boundary**: Diagnostic tools (`ratana-hardware-probe`) verify kernel driver initialization, audio server endpoints (PipeWire sinks/sources), camera nodes (`/dev/video*`), and printer queues (CUPS).
- **Guidance**: Physical verification (e.g. printing a physical test page or testing gamepad analog sticks) depends on attaching physical hardware devices. Subsystem readiness is verified in all standard environments.

## 5. Modern Wi-Fi 7 Firmware Backports
- **Guidance**: If your hardware uses a brand-new Wi-Fi 7 chipset released after Debian Bookworm, ensure your system is connected via Ethernet during initial setup so `ratana-driver-manager` can fetch backported firmware packages.

## 6. Hibernation (Suspend-to-Disk) Requirements
- **Guidance**: System suspend (S3 / S0ix sleep) is supported out of the box. Hibernation (`systemctl hibernate`) requires a swap partition or swap file equal to or greater than the installed RAM capacity.
