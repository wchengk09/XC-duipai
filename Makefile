CXX      := g++
CXXFLAGS := -Wall -Wextra -O2 -Wno-unused-result -Wno-write-strings -Wno-literal-suffix -g3 -std=gnu++17
# 用户自定义宏定义，例如 make EXTRA_DEFS='-Dkevin -DDEBUG'
# 持久化可在 shell 执行：export EXTRA_DEFS=-Dkevin
# 也可通过项目根目录下的 .env 文件添加（不会提交到 git）
-include .env
EXTRA_DEFS ?=
CPPFLAGS := -I./lib/include $(EXTRA_DEFS)
LDFLAGS  := -L./lib/lib -lreadline -lpthread -lncursesw

# 主程序源文件与自动生成的依赖
SRCS := main.cpp src/common.cpp src/parse.cpp src/config.cpp src/complete.cpp src/run.cpp src/commands.cpp
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean run init init-wa init-std init-rand init-spj init-conf

all: main

# 主程序：链接所有目标文件（头文件依赖由 -MMD -MP 自动生成）
main: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) -o main $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ -MMD -MP

-include $(DEPS)

# 用户对拍程序
run: std wa rand

std: std.cpp
	$(CXX) $(CXXFLAGS) std.cpp -o std

wa: wa.cpp
	$(CXX) $(CXXFLAGS) wa.cpp -o wa

rand: rand.cpp
	$(CXX) $(CXXFLAGS) rand.cpp -o rand

spj: spj.cpp
	$(CXX) $(CXXFLAGS) spj.cpp -o spj

clean:
	rm -f main std rand wa spj $(OBJS) $(DEPS) gmon.out prof.txt xor2.out \
	      wa-*.txt in-*.txt std-*.txt in.txt wa.txt std.txt
	rm -rf csd

init: init-wa init-std init-rand init-spj init-conf

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
