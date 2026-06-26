#!/bin/sh
# XC-duipai 分支管理辅助脚本
#
# 用法：
#   scripts/branch.sh <name>       # 从当前分支创建并切换到 <name>
#   scripts/branch.sh list         # 列出本地分支
#   scripts/branch.sh switch <name># 切换到 <name>
#   scripts/branch.sh done <name>  # 切回主干(main/master)并合并 <name>
set -e

cd "$(cd "$(dirname "$0")/.." && pwd)"

case "${1:-}" in
    list)
        git branch -vv
        ;;
    switch)
        [ -n "$2" ] || { echo "Usage: $0 switch <name>" >&2; exit 1; }
        git switch "$2"
        ;;
    done)
        [ -n "$2" ] || { echo "Usage: $0 done <branch>" >&2; exit 1; }
        main=$(git rev-parse --verify main >/dev/null 2>&1 && echo main || echo master)
        git switch "$main"
        git merge --no-ff "$2"
        echo "Merged $2 into $main. Delete branch with: git branch -d $2"
        ;;
    "")
        echo "Usage: $0 <name|list|switch <name>|done <branch>>" >&2
        exit 1
        ;;
    *)
        # 创建并切换到新分支
        git switch -c "$1"
        ;;
esac
