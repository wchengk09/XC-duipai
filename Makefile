CPPFLAGS=-Wall -Wextra -Dkevin -O2 -Wno-unused-result -Wno-write-strings -Wno-literal-suffix
.PHONY: all clean

all: main

run: std wa rand

main: main.cpp
	g++ main.cpp -o main $(CPPFLAGS) -I. -L./lib/lib -lreadline -lpthread -lncursesw -g3

clean:
	rm -rf main std rand wa wa-*.txt std-*.txt in-*.txt csd/* in.txt wa.txt std.txt

std: std.cpp
	g++ std.cpp -o std $(CPPFLAGS)

wa: wa.cpp
	g++ wa.cpp -o wa $(CPPFLAGS)

rand: rand.cpp
	g++ rand.cpp -o rand $(CPPFLAGS)

spj: spj.cpp
	g++ spj.cpp -o spj $(CPPFLAGS)

init-wa:
	cp src/templates/stdwa.cpp wa.cpp

init-std:
	cp src/templates/stdwa.cpp std.cpp

init-rand:
	cp src/templates/rand.cpp rand.cpp

init-spj:
	cp src/templates/spj.cpp spj.cpp

init-conf:
	cp src/templates/config config

init: init-wa init-std init-rand init-spj init-conf	