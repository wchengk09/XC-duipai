#include "commands.h"

#include "config.h"
#include "parse.h"
#include "run.h"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>

namespace commands {

static bool has_flag(const cmdlist& cmd, const std::string& flag) {
    return std::find(cmd.begin(), cmd.end(), flag) != cmd.end();
}

// 在时限/空限约束下运行程序，失败时打印并抛出 XCException
static int run_with_status(const cmdlist& cmd, const std::string& in,
                           const std::string& out) {
    int ret = exe(cmd, in, out, "/dev/stderr", Config::time_limit, Config::mem_limit);
    if (ret == TLE) {
        std::printf(RED "Time limit eXCeeded\n" ZERO);
        throw XCException("tle");
    } else if (ret == MLE) {
        std::printf(RED "Memory limit eXCeeded\n" ZERO);
        throw XCException("mle");
    } else if (ret) {
        std::printf(RED "Runtime error\n" ZERO);
        throw XCException("re");
    }
    return ret;
}

static void cmd_clear(const cmdlist&) { clear(); }

static void cmd_threads(const cmdlist& cmd) {
    Config::threads = Parse::range(cmd, 1, 1, MAX_THREADS);
    Config::save();
}

static void cmd_time(const cmdlist& cmd) {
    Config::time_limit = Parse::range(cmd, 1, -1, INT_MAX / 1000);
    Config::save();
}

static void cmd_mem(const cmdlist& cmd) {
    Config::mem_limit = Parse::range(cmd, 1, -1, INT_MAX >> 20);
    Config::save();
}

static void cmd_getconf(const cmdlist&) {
    std::printf("Time limit:" PURPLE " %dms" ZERO "\n", Config::time_limit);
    std::printf("Memory limit:" PURPLE " %dMB" ZERO "\n", Config::mem_limit);
    std::printf("Threads:" PURPLE " %d" ZERO "\n", Config::threads);
}

static void cmd_quit(const cmdlist&) { throw QuitException(); }

static void cmd_run(const cmdlist& cmd) {
    bool t = has_flag(cmd, "-t");
    if (exe({"/bin/make", "-j", "run"})) throw XCException("build");
    if (t) {
        std::printf(RED "in.txt:" ZERO "\n");
        if (exe({"/bin/cat", "in.txt"})) throw XCException("cat");
        std::printf(RED "\nwa.txt:" ZERO "\n");
        run_with_status({"./wa"}, "in.txt", "wa.txt");
        if (exe({"/bin/cat", "wa.txt"})) throw XCException("cat");
        std::printf(RED "\nstd.txt:" ZERO "\n");
        run_with_status({"./std"}, "in.txt", "std.txt");
        if (exe({"/bin/cat", "std.txt"})) throw XCException("cat");
        std::printf("\n");
    } else {
        RUN::run(DUIPAI);
    }
}

static void cmd_spjcheck(const cmdlist& cmd) {
    bool t = has_flag(cmd, "-t");
    if (exe({"/bin/make", "-j", "wa", "rand", "spj"})) throw XCException("build");
    if (t) {
        std::printf(RED "in.txt:" ZERO "\n");
        if (exe({"/bin/cat", "in.txt"})) throw XCException("cat");
        std::printf(RED "\nwa.txt:" ZERO "\n");
        run_with_status({"./wa"}, "in.txt", "wa.txt");
        if (exe({"/bin/cat", "wa.txt"})) throw XCException("cat");
        std::printf(RED "\nSPJ output:" ZERO "\n");
        int ret = exe({"./spj", "in.txt", "wa.txt", "wa.txt"});
        std::printf("\n");
        std::printf("SPJ Returned " LBLUE "%d\n" ZERO, ret);
    } else {
        RUN::run(SPJCHECK);
    }
}

static void cmd_spjcmp(const cmdlist& cmd) {
    bool t = has_flag(cmd, "-t");
    if (exe({"/bin/make", "-j", "run", "spj"})) throw XCException("build");
    if (t) {
        std::printf(RED "in.txt:" ZERO "\n");
        if (exe({"/bin/cat", "in.txt"})) throw XCException("cat");
        std::printf(RED "\nwa.txt:" ZERO "\n");
        run_with_status({"./wa"}, "in.txt", "wa.txt");
        if (exe({"/bin/cat", "wa.txt"})) throw XCException("cat");
        std::printf(RED "\nstd.txt:" ZERO "\n");
        run_with_status({"./std"}, "in.txt", "std.txt");
        if (exe({"/bin/cat", "std.txt"})) throw XCException("cat");
        std::printf("\n");
        std::printf(RED "SPJ output:\n\n" ZERO);
        if (exe({"./spj", "in.txt", "wa.txt", "std.txt"})) throw XCException("spj");
    } else {
        RUN::run(SPJCMP);
    }
}

static void cmd_tle(const cmdlist& cmd) {
    bool t = has_flag(cmd, "-t");
    if (exe({"/bin/make", "-j", "rand", "wa"})) throw XCException("build");
    if (t) {
        std::printf(RED "in.txt:" ZERO "\n\n");
        if (exe({"/bin/cat", "in.txt"})) throw XCException("cat");
        std::printf("\n\n");
    } else {
        RUN::run(DETECT);
    }
}

static void cmd_gen(const cmdlist& cmd) {
    // gen hack <n> [-t] [-a] [-o] [-m <max_attempts>]
    if (cmd.size() >= 2 && cmd[1] == "hack") {
        int n = Parse::range(cmd, 2, 1, INT_MAX);
        bool t = has_flag(cmd, "-t");
        bool a = has_flag(cmd, "-a");
        bool o = has_flag(cmd, "-o");
        int m = -1;  // 默认 +inf（无限制）
        for (size_t i = 3; i < cmd.size(); i++) {
            if (cmd[i] == "-m") {
                if (i + 1 >= cmd.size()) {
                    std::printf(RED "Error:" ZERO " -m requires a value!\n");
                    throw XCException("missing value");
                }
                m = Parse::range(cmd, i + 1, -1, INT_MAX);
                break;
            }
        }
        auto hacks = RUN::list_hacks();
        if (hacks.empty()) {
            std::printf(RED "Error:" ZERO " no hack solutions found in " YELLOW "hacks/" ZERO
                        " directory.\n");
            std::printf("Please create " YELLOW "hacks/*.cpp" ZERO " files"
                        " (e.g. " PURPLE "make init-hack" ZERO ").\n");
            throw XCException("no hacks");
        }
        if (exe({"/bin/make", "-j", "hack"})) throw XCException("build");
        if (t) {
            // 测试模式：跑一次 rand(1) → in.txt → std → 每个 hack，展示输出与比较结果
            std::printf(RED "./rand output:\n\n" ZERO);
            if (exe({"./rand", "1"}, "/dev/stdin", "in.txt"))
                throw XCException("rand");
            if (exe({"/bin/cat", "in.txt"})) throw XCException("cat");
            std::printf("\n");
            std::printf(RED "./std output:\n" ZERO);
            if (exe({"./std"}, "in.txt", "std.txt")) throw XCException("std");
            if (exe({"/bin/cat", "std.txt"})) throw XCException("cat");
            std::printf("\n");
            for (const auto& name : hacks) {
                std::string hack_out = "hack-" + name + ".txt";
                std::printf(RED "./hacks/%s:\n" ZERO, name.c_str());
                int ret = exe({"./hacks/" + name}, "in.txt", hack_out,
                              "/dev/null", Config::time_limit, Config::mem_limit);
                if (ret == TLE) {
                    std::printf(RED "  Time limit eXCeeded → HACKED\n" ZERO);
                } else if (ret == MLE) {
                    std::printf(RED "  Memory limit eXCeeded → HACKED\n" ZERO);
                } else if (ret) {
                    std::printf(RED "  Runtime error (exit %d) → HACKED\n" ZERO, ret);
                } else {
                    if (exe({"/bin/cat", hack_out.c_str()})) throw XCException("cat");
                    if (exe({"/bin/diff", "-Z", hack_out, "std.txt"},
                            "/dev/null", "/dev/null", "/dev/null")) {
                        std::printf(RED "  Output differs from std → HACKED\n" ZERO);
                    } else {
                        std::printf(GREEN "  Output matches std → not hacked\n" ZERO);
                    }
                }
                std::printf("\n");
            }
        } else {
            RUN::hack_main(n, a, o, m);
        }
        return;
    }
    // gen <n> [-t]
    int n = Parse::range(cmd, 1, 1, INT_MAX);
    bool t = has_flag(cmd, "-t");
    if (exe({"/bin/make", "-j", "rand", "std"})) throw XCException("build");
    if (t) {
        std::printf(RED "./rand output:\n\n" ZERO);
        if (exe({"./rand", std::to_string(n)}, "/dev/stdin", "in.txt"))
            throw XCException("rand");
        if (exe({"/bin/cat", "in.txt"})) throw XCException("cat");
        std::printf("\n");
        std::printf(RED "./std output:\n" ZERO);
        if (exe({"./std"}, "in.txt")) throw XCException("std");
        std::printf("\n");
    } else {
        RUN::gen_main(n);
    }
}

static void cmd_xc(const cmdlist&) {
    std::printf(RED "LONG LIVE XC!!!\n\n" ZERO);
    std::printf(YELLOW
        "*     *     *******\n"
        " *   *     *       \n"
        "  * *      *       \n"
        "   *       *       \n"
        "  * *      *       \n"
        " *   *     *       \n"
        "*     *     *******\n"
        ZERO);
}

static void cmd_cd(const cmdlist& cmd) {
    Parse::least(cmd, 1);
    if (chdir(cmd[1].c_str()) != 0)
        std::perror(("cd: " + cmd[1]).c_str());
}

using Handler = void (*)(const cmdlist&);
struct Entry {
    const char* name;
    Handler handler;
};

static const Entry table[] = {
    {"clear", cmd_clear},
    {"threads", cmd_threads},
    {"time", cmd_time},
    {"mem", cmd_mem},
    {"getconf", cmd_getconf},
    {"exit", cmd_quit},
    {"quit", cmd_quit},
    {"run", cmd_run},
    {"spjcheck", cmd_spjcheck},
    {"spjcmp", cmd_spjcmp},
    {"tle", cmd_tle},
    {"gen", cmd_gen},
    {"xc", cmd_xc},
    {"XC", cmd_xc},
    {"cd", cmd_cd},
};

bool dispatch(const cmdlist& cmd) {
    for (const auto& e : table) {
        if (cmd[0] == e.name) {
            e.handler(cmd);
            return true;
        }
    }
    return false;
}

}  // namespace commands
