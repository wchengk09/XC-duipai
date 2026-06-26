#include "complete.h"

#include <cstdlib>
#include <cstring>

namespace Complete {

// 支持的内置命令列表（须与 commands.cpp 的分发表保持同步）
static const char* const commands[] = {
    "run", "spjcmp", "spjcheck", "tle", "gen",
    "time", "mem", "threads", "getconf",
    "exit", "quit", "clear", "cd", "xc", "XC",
    nullptr};

static char* dupstr(const char* s) {
    char* r = static_cast<char*>(std::malloc(std::strlen(s) + 1));
    std::strcpy(r, s);
    return r;
}

static char* command_generator(const char* text, int state) {
    static int list_index, len;
    if (!state) {
        list_index = 0;
        len = std::strlen(text);
    }
    const char* name;
    while ((name = commands[list_index])) {
        list_index++;
        if (std::strncmp(name, text, len) == 0)
            return dupstr(name);
    }
    return nullptr;
}

char** fileman_completion(const char* text, int start, int) {
    if (start == 0)
        return rl_completion_matches(text, command_generator);
    return nullptr;
}

}  // namespace Complete
