#!/bin/sh
# 打包源码发布 tarball（不执行任何 git 推送，git 操作请用 scripts/release.sh）
set -e

cd "$(dirname "$0")"
make clean

# 临时移走用户对拍代码，使用模板打包
tmp=$(mktemp -d)
for f in std.cpp wa.cpp rand.cpp spj.cpp; do
    [ -f "$f" ] && mv "$f" "$tmp/$f"
done
make init

tar --exclude=ncurses --exclude=readline --exclude=lib \
    --exclude='*.txt' --exclude='*.o' --exclude='*.d' \
    --exclude=rand --exclude=std --exclude=wa --exclude=main --exclude=spj \
    -zcvf "../XC-duipai-$(date '+%Y%m%d%H%M%S').tar.gz" .

# 恢复用户对拍代码
for f in std.cpp wa.cpp rand.cpp spj.cpp; do
    [ -f "$tmp/$f" ] && mv "$tmp/$f" "$f"
done
rm -rf "$tmp"

echo "Packaged. Commit/push via scripts/release.sh or git manually."
