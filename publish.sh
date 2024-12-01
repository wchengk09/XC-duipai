#!/bin/sh
make clean
tmp=/tmp/XC-duipai-cache
mkdir -p $tmp
mv std.cpp $tmp/std.cpp
mv wa.cpp $tmp/wa.cpp
mv rand.cpp $tmp/rand.cpp
mv spj.cpp $tmp/spj.cpp
make init
git add .
git commit
git push origin master -f
tar --exclude=ncurses --exclude=readline --exclude=*.txt --exclude=rand --exclude=std --exclude=wa --exclude=main --exclude=lib -zcvf ../XC-duipai-$(date "+%Y%m%d%H%M%S").tar.gz * | head -n 100
mv $tmp/std.cpp std.cpp 
mv $tmp/wa.cpp wa.cpp 
mv $tmp/rand.cpp rand.cpp 
mv $tmp/spj.cpp spj.cpp 