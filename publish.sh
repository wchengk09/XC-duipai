#!/bin/sh
make clean
git add .
git commit
git push origin master -f
tar --exclude=ncurses --exclude=readline --exclude=*.txt --exclude=rand --exclude=std --exclude=wa -zcvf ../XC-duipai-$(date "+%Y%m%d%H%M%S").tar.gz * | head -n 100