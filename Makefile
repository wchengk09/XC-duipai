CPPFLAGS=-Wall -Wextra -Dkevin -O2 -Wno-unused-result -Wno-write-strings -Wno-literal-suffix
CLEAN=cat /dev/null > 
.PHONY: all readline san clean push

all: main

run: std wa rand

main: main.cpp
	g++ main.cpp -o main $(CPPFLAGS) -I. -L./lib/lib -lreadline -lpthread -lncursesw -g3

clean:
	rm -rf main std rand wa-*.txt std-*.txt in-*.txt csd/* ncurses readline lib

# 请不要随意执行make push，否则你的代码将会丢失
push: clean
	$(CLEAN) wa.cpp
	$(CLEAN) std.cpp
	$(CLEAN) rand.cpp

std: std.cpp
	g++ std.cpp -o std $(CPPFLAGS)

wa: wa.cpp
	g++ wa.cpp -o wa $(CPPFLAGS)

rand: rand.cpp
	g++ rand.cpp -o rand $(CPPFLAGS)