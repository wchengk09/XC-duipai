#!/bin/sh
make clean
git add .
git commit
git push origin master -f
tar zcvf ../XC-duipai-$(date "+%Y%m%d%H%M%S").tar.gz * --exclude=ncurses --exclude=readline --exclude=*.txt --exclude=rand --exclude=std --exclude=wa