#!/bin/sh
INSTDIR=$(pwd)/lib
mkdir $INSTDIR
wget -O readline.tar.gz https://mirrors.aliyun.com/gnu/readline/readline-8.2.tar.gz
wget -O ncurses.tar.gz http://ftp.gnu.org/pub/gnu/ncurses/ncurses-6.5.tar.gz
tar xvf readline.tar.gz
tar xvf ncurses.tar.gz
rm readline.tar.gz ncurses.tar.gz
mv ncurses-* ncurses
mv readline-* readline
cd ncurses
./configure --with-shared --prefix=$INSTDIR --without-manpages
make -j
make install -j
cd ..
cd readline
./configure --prefix=$INSTDIR --without-manpages
make -j
make install -j
cd ..