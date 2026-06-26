#include "parse.h"

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <climits>

namespace Parse {

bool is_number(const std::string& s) {
    if (s.empty()) return false;
    std::size_t i = (s[0] == '-') ? 1 : 0;
    if (i == s.size()) return false;  // 全横线，非合法数字
    for (; i < s.size(); ++i)
        if (!std::isdigit((unsigned char)s[i])) return false;
    return true;
}

void least(const cmdlist& cmd, int id) {
    if ((int)cmd.size() <= id) {
        std::printf(RED "Error:" ZERO " command " YELLOW "%s" ZERO
                    " requires at least " PURPLE "%d" ZERO " argument(s)!\n",
                    cmd[0].c_str(), id);
        throw XCException("missing argument");
    }
}

int number(const cmdlist& cmd, int id) {
    least(cmd, id);
    if (!is_number(cmd[id])) {
        std::printf(RED "Error:" LBLUE " %s" ZERO " is not a number!\n",
                    cmd[id].c_str());
        throw XCException("not a number");
    }
    // 用 strtol 替代 atoi，检测溢出（atoi 对超 int 范围输入是 UB）
    errno = 0;
    long val = std::strtol(cmd[id].c_str(), nullptr, 10);
    if (errno == ERANGE || val < INT_MIN || val > INT_MAX) {
        std::printf(RED "Error:" LBLUE " %s" ZERO " is out of int range!\n",
                    cmd[id].c_str());
        throw XCException("out of int range");
    }
    return (int)val;
}

int range(const cmdlist& cmd, int id, int l, int r) {
    int x = number(cmd, id);
    if (x < l || x > r) {
        std::printf(RED "Error:" LBLUE " %d" ZERO " is not in the range "
                    PURPLE "[%d,%d]" ZERO "!\n",
                    x, l, r);
        throw XCException("out of range");
    }
    return x;
}

}  // namespace Parse
