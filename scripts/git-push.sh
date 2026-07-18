#!/bin/bash
# ================================================================
# RatanaOS v5.0 "Phoenix" — Git Commit & GitHub Push Script
#
# Run this script on your local machine where git IS installed.
# This will commit all source code and push to GitHub.
#
# Usage:
#   1. Set your GitHub username below OR pass as argument
#   2. Run: chmod +x scripts/git-push.sh && ./scripts/git-push.sh
#
# Example:
#   GITHUB_USER=ratanazen ./scripts/git-push.sh
# ================================================================

set -e

GITHUB_USER=${GITHUB_USER:-"your-username"}    # ← Set your GitHub username here
REPO_NAME="RatanaOS"
BRANCH="feature/v5.0-phoenix"
VERSION="v5.0.0"

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

echo "=========================================="
echo "  RatanaOS Git Commit & Push             "
echo "=========================================="
echo "  User   : ${GITHUB_USER}"
echo "  Repo   : ${REPO_NAME}"
echo "  Branch : ${BRANCH}"
echo "  Version: ${VERSION}"
echo ""

# ── 1. Configure git identity ───────────────────────────────────────
git config user.name  "RatanaOS Builder"
git config user.email "builder@ratanaos.local"

# ── 2. Initialize if not already a git repo ─────────────────────────
if [ ! -d ".git" ]; then
  echo "[1] Initializing new git repository..."
  git init
  git remote add origin "https://github.com/${GITHUB_USER}/${REPO_NAME}.git"
else
  echo "[1] Git repository already initialized."
  # Update remote if needed
  git remote set-url origin "https://github.com/${GITHUB_USER}/${REPO_NAME}.git" 2>/dev/null || \
    git remote add origin "https://github.com/${GITHUB_USER}/${REPO_NAME}.git"
fi

# ── 3. Create and switch to feature branch ──────────────────────────
echo "[2] Creating branch: ${BRANCH}"
git checkout -b "${BRANCH}" 2>/dev/null || git checkout "${BRANCH}"

# ── 4. Stage all source code (ISOs excluded by .gitignore) ──────────
echo "[3] Staging all source files..."
git add -A
echo "Files staged:"
git status --short | head -60

# ── 5. Commit ───────────────────────────────────────────────────────
echo ""
echo "[4] Committing..."
git commit -m "feat: RatanaOS v5.0.0 Phoenix — complete OS release

Includes:
- Build system: 6 editions (Lite/Standard/Developer/Cyber/Server/ARM64)
- ISO builder with 11-step pipeline and profile JSON configs
- Qt6 installer with 11 wizard steps incl. Edition + Desktop + Update Strategy
- GitHub Actions CI/CD (multi-arch builds, Trivy scan, auto-release)
- All core applications: Terminal, Settings, Software Center,
  Update Manager, System Monitor, File Manager, Welcome,
  AI Assistant, Firewall, Recovery
- Control Center: 12 panels (Appearance through About)
- Welcome app: all 5 OOBE pages implemented
- Ratana Firewall + Ratana Recovery apps (new in v5.0)
- Profiles JSON: ratana-lite, standard, developer, cyber, server, arm
- Roadmap: v1.0 through v10.0 LTS documented
- SECURITY.md, ACCESSIBILITY.md, MIGRATION_GUIDE.md
- QA reports: QA_REPORT.md, BOOT_REPORT.md, INSTALL_REPORT.md
" || echo "Nothing new to commit."

# ── 6. Push to GitHub ───────────────────────────────────────────────
echo ""
echo "[5] Pushing to GitHub..."
echo "    Repository: https://github.com/${GITHUB_USER}/${REPO_NAME}"
echo "    Branch    : ${BRANCH}"
echo ""
git push -u origin "${BRANCH}"

echo ""
echo "=========================================="
echo " ✅ Push complete!                        "
echo "   1. Go to GitHub and open a PR from    "
echo "      ${BRANCH} → main                   "
echo "   2. After merging, the CI workflow will "
echo "      auto-build ISOs and draft a Release "
echo "   3. Or publish manually with:           "
echo "      GITHUB_TOKEN=ghp_xxx ./scripts/release.sh ${VERSION}"
echo "=========================================="
