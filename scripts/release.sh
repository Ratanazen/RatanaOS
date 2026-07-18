#!/bin/bash
# RatanaOS GitHub Release Publisher
# Usage: ./scripts/release.sh <version> <github_token>
# Example: ./scripts/release.sh v5.0.0 ghp_xxxxxxxxxxxx

set -e

VERSION=${1:-"v5.0.0"}
GITHUB_TOKEN=${2:-"${GITHUB_TOKEN}"}
REPO=${GITHUB_REPO:-"your-username/RatanaOS"}   # ← Set this to your real GitHub repo
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUTPUT_DIR="${ROOT_DIR}/output"

if [ -z "$GITHUB_TOKEN" ]; then
  echo "❌ Error: GITHUB_TOKEN not set."
  echo "   Usage: GITHUB_TOKEN=ghp_xxx ./scripts/release.sh v5.0.0"
  exit 1
fi

echo "=========================================="
echo "  RatanaOS GitHub Release Publisher       "
echo "=========================================="
echo "  Version : ${VERSION}"
echo "  Repo    : ${REPO}"
echo ""

# ── Step 1: Build all ISOs if not already built ─────────────────────
echo "[1/4] Verifying build artifacts..."
REQUIRED=("RatanaOS-Lite.iso" "RatanaOS-Standard.iso" "RatanaOS-Developer.iso" "RatanaOS-Cyber.iso" "RatanaOS-Server.iso" "RatanaOS-ARM64.img")
MISSING=0
for f in "${REQUIRED[@]}"; do
  if [ ! -f "${OUTPUT_DIR}/${f}" ]; then
    echo "  ⚠️  Missing: ${f}"
    MISSING=$((MISSING + 1))
  else
    echo "  ✅ Found: ${f}"
  fi
done

if [ "$MISSING" -gt 0 ]; then
  echo ""
  echo "Building missing artifacts..."
  for profile in ratana-lite ratana-standard ratana-developer ratana-cyber ratana-server ratana-arm; do
    "${ROOT_DIR}/builder/build-iso.sh" "$profile"
  done
fi

# ── Step 2: Regenerate checksums ────────────────────────────────────
echo ""
echo "[2/4] Generating SHA256SUMS and SHA512SUMS..."
cd "${OUTPUT_DIR}"
sha256sum RatanaOS-*.iso RatanaOS-*.img > SHA256SUMS 2>/dev/null || true
sha512sum RatanaOS-*.iso RatanaOS-*.img > SHA512SUMS 2>/dev/null || true
echo "  ✅ Checksums written."

# ── Step 3: Create GitHub Release ───────────────────────────────────
echo ""
echo "[3/4] Creating GitHub Release ${VERSION}..."

RELEASE_BODY=$(cat <<EOF
## RatanaOS ${VERSION}

$(cat "${ROOT_DIR}/docs/ROADMAP.md" | grep -A 30 "In Development" | head -20)

---

### 📥 Download

| Edition | File | Size | Purpose |
|---|---|---|---|
| Lite | \`RatanaOS-Lite.iso\` | ≈2 GB | XFCE, older hardware |
| Standard | \`RatanaOS-Standard.iso\` | ≈3 GB | KDE Plasma, everyday use |
| Developer | \`RatanaOS-Developer.iso\` | ≈3.5 GB | KDE + full dev toolchain |
| Cyber | \`RatanaOS-Cyber.iso\` | ≈4 GB | KDE + optional security tools |
| Server | \`RatanaOS-Server.iso\` | ≈800 MB | Headless, Docker, hardened |
| ARM64 | \`RatanaOS-ARM64.img\` | ≈2 GB | Raspberry Pi 4/5, ARM SBCs |

### 🔐 Verification
\`\`\`bash
sha256sum -c SHA256SUMS
\`\`\`

### 📖 Documentation
- [Installation Guide](https://github.com/${REPO}/blob/main/docs/INSTALL.md)
- [Build Guide](https://github.com/${REPO}/blob/main/docs/BUILD.md)
- [Security Policy](https://github.com/${REPO}/blob/main/SECURITY.md)
EOF
)

# Create the release via GitHub API
RELEASE_RESPONSE=$(curl -s -X POST \
  -H "Authorization: token ${GITHUB_TOKEN}" \
  -H "Content-Type: application/json" \
  "https://api.github.com/repos/${REPO}/releases" \
  -d "{
    \"tag_name\": \"${VERSION}\",
    \"name\": \"RatanaOS ${VERSION} Phoenix\",
    \"body\": $(echo "$RELEASE_BODY" | python3 -c 'import json,sys; print(json.dumps(sys.stdin.read()))'),
    \"draft\": true,
    \"prerelease\": false
  }")

UPLOAD_URL=$(echo "$RELEASE_RESPONSE" | python3 -c "import json,sys; d=json.load(sys.stdin); print(d.get('upload_url','').replace('{?name,label}',''))" 2>/dev/null || echo "")

if [ -z "$UPLOAD_URL" ]; then
  echo "  ❌ Failed to create GitHub Release. Check your GITHUB_TOKEN and REPO."
  echo "  Response: ${RELEASE_RESPONSE}"
  exit 1
fi

echo "  ✅ Draft release created."

# ── Step 4: Upload all artifacts ────────────────────────────────────
echo ""
echo "[4/4] Uploading artifacts..."
for artifact in RatanaOS-*.iso RatanaOS-*.img SHA256SUMS SHA512SUMS; do
  if [ -f "${OUTPUT_DIR}/${artifact}" ]; then
    echo "  Uploading: ${artifact}"
    curl -s -X POST \
      -H "Authorization: token ${GITHUB_TOKEN}" \
      -H "Content-Type: application/octet-stream" \
      "${UPLOAD_URL}?name=${artifact}" \
      --data-binary "@${OUTPUT_DIR}/${artifact}" > /dev/null && echo "  ✅ Uploaded: ${artifact}"
  fi
done

echo ""
echo "=========================================="
echo " 🎉 Release ${VERSION} created as DRAFT!  "
echo " Go to GitHub → Releases to review and    "
echo " publish it when ready.                   "
echo "=========================================="
