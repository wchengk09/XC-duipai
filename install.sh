#!/bin/sh
# 安装 readline 与 ncurses 到项目内 lib/ 目录
set -e

INSTDIR="$(pwd)/lib"
mkdir -p "$INSTDIR"

# 需要预先放置 readline.tar.gz 与 ncurses.tar.gz
tar xf readline.tar.gz
tar xf ncurses.tar.gz
mv ncurses-* ncurses
mv readline-* readline

cd ncurses
./configure --with-shared --prefix="$INSTDIR" --without-manpages
make -j
make install
cd ../readline
./configure --prefix="$INSTDIR" --without-manpages
make -j
make install

echo "Done. Libraries installed to $INSTDIR"
