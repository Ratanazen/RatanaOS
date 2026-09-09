# 🚀 RatanaOS Release Process (Phase 12)

This document specifies the exact steps from code change to tagged,
checksummed, boot-tested ISO publication.

---

## Pre-Release Checklist

1. **Code Freeze**: Merge all feature branches into \`main\`.
2. **Local Build**: Run \`make live-iso\` locally to verify the ISO builds
   without errors.
3. **Local Boot Test**: Run \`make live-run\` and verify:
   - Desktop boots to XFCE with RatanaOS theme.
   - Network connectivity (DHCP).
   - Audio device visible (\`aplay -l\`).
   - Calamares installer launches.
4. **Parity Checklist**: Verify all 16 items in \`docs/PARITY_CHECKLIST.md\`
   pass.

---

## Release Steps

### 1. Create Release Branch
\`\`\`bash
git checkout -b release/v2.0.1
git push origin release/v2.0.1
\`\`\`
This triggers the GitHub Actions CI build automatically.

### 2. CI Build (Automated)
The \`.github/workflows/build-iso.yml\` workflow:
- Runs inside a privileged \`debian:bookworm\` container.
- Executes \`lb config\` + \`lb build\`.
- Computes SHA-256 checksum.
- Uploads the ISO and checksum as build artifacts.

### 3. Manual Boot Test (Required)
> [!IMPORTANT]
> CI runners typically do NOT support nested KVM virtualization.
> QEMU boot testing is a **manual** step performed locally before
> tagging a release.

Download the CI-built ISO artifact and test:
\`\`\`bash
# UEFI boot test
qemu-system-x86_64 -bios /usr/share/OVMF/OVMF_CODE.fd \\
  -cdrom ratanaos-live-amd64.hybrid.iso -m 2048 -smp 2

# BIOS boot test
qemu-system-x86_64 \\
  -cdrom ratanaos-live-amd64.hybrid.iso -m 2048 -smp 2
\`\`\`

### 4. Tag Release
\`\`\`bash
git tag -a v2.0.1 -m "RatanaOS 2.0.1 — Debian Bookworm desktop release"
git push origin v2.0.1
\`\`\`

### 5. Publish Release
- Create a GitHub Release from the tag.
- Attach the ISO and \`.sha256\` checksum file.
- Include release notes from \`CHANGELOG.md\`.
