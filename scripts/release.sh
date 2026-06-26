#!/bin/sh
# XC-duipai 版本管理脚本
#
# 用法：
#   scripts/release.sh [major|minor|patch]   # bump 版本号、提交、打 tag，询问是否推送
#   scripts/release.sh status                # 查看当前版本与最近 tag
#   scripts/branch.sh <name>                 # 见 scripts/branch.sh
#
# 不会强制推送（不使用 -f），保护远端历史。
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

VERSION_FILE="$ROOT/VERSION"
[ -f "$VERSION_FILE" ] || { echo "Error: VERSION file not found" >&2; exit 1; }

CUR=$(tr -d '[:space:]' < "$VERSION_FILE")

show_status() {
    echo "Current version: $CUR"
    echo "Latest tags:"
    git tag --sort=-v:refname 2>/dev/null | head -5 || echo "  (no tags)"
    echo "Current branch: $(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo '(not a git repo)')"
}

bump() {
    case "$1" in
        major|minor|patch) ;;
        *) echo "Usage: $0 [major|minor|patch|status]" >&2; exit 1 ;;
    esac
    MAJOR=$(echo "$CUR" | cut -d. -f1)
    MINOR=$(echo "$CUR" | cut -d. -f2)
    PATCH=$(echo "$CUR" | cut -d. -f3)
    case "$1" in
        major) MAJOR=$((MAJOR+1)); MINOR=0; PATCH=0 ;;
        minor) MINOR=$((MINOR+1)); PATCH=0 ;;
        patch) PATCH=$((PATCH+1)) ;;
    esac
    NEW="$MAJOR.$MINOR.$PATCH"
    echo "Bump: $CUR -> $NEW"
    printf '%s\n' "$NEW" > "$VERSION_FILE"
    git add VERSION
    git commit -m "release: v$NEW"
    git tag "v$NEW"
    echo "Created commit and tag v$NEW."
    printf "Push to origin? [y/N] "
    read ans
    case "$ans" in
        y|Y)
            git push origin HEAD
            git push origin "v$NEW"
            echo "Pushed."
            ;;
        *)
            echo "Skipped push. Run manually:"
            echo "  git push origin HEAD && git push origin v$NEW"
            ;;
    esac
}

case "${1:-patch}" in
    status) show_status ;;
    *) bump "$1" ;;
esac
