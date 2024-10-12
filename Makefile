CPPFLAGS=-Wall -Wextra -Dkevin -O2 -Wno-unused-result -Wno-write-strings -Wno-literal-suffix
CLEAN=cat /dev/null > 
.PHONY: all readline san clean push

all: main

run: std wa rand

main: main.cpp
	g++ main.cpp -o main $(CPPFLAGS) -I. -L./lib/lib -lreadline -lpthread -lncursesw -g3

clean:
	rm -rf main std rand wa-*.txt std-*.txt in-*.txt csd/* in.txt wa.txt std.txt

std: std.cpp
	g++ std.cpp -o std $(CPPFLAGS)

wa: wa.cpp
	g++ wa.cpp -o wa $(CPPFLAGS)

rand: rand.cpp
	g++ rand.cpp -o rand $(CPPFLAGS)