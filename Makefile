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

init:
	cp src/templates/rand.cpp rand.cpp
	cp src/templates/stdwa.cpp wa.cpp
	cp wa.cpp std.cpp
	cp src/templates/spj.cpp spj.cpp
	cp src/templates/config config