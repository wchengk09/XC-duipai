#include "run.h"

#include "common.h"
#include "config.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>

namespace RUN {

static std::mutex finish;
static std::vector<std::thread> thr;
static std::vector<int> cnt;
static std::string mess;
static int id = 0;

static void End(std::string message, int tid) {
    std::lock_guard<std::mutex> lk(finish);
    if (killed) return;
    killed = true;
    mess = std::move(message);  // 在锁内修改，避免数据竞争
    id = tid;
}

static void newsigint(int) {
    End(RED "Interrupted." ZERO, 0);
}

static void init() {
    killed = false;
    signal(SIGINT, newsigint);
    cnt.assign(Config::threads + 1, 0);
    id = 0;
    mess.clear();
}

static int finished_count() {
    int ans = 0;
    for (int i = 1; i <= Config::threads; i++)
        ans += cnt[i];
    return ans;
}

static void run_one(const cmdlist& cmd, const std::string& in,
                    const std::string& out, int tl, int ml, int tid) {
    int ret = exe(cmd, in, out, "/dev/stderr", tl, ml);
    if (ret == TLE)
        End(std::string(YELLOW) + cmd[0] + RED " Time limit eXCeeded" ZERO
                " on testcase " LBLUE + std::to_string(finished_count()) + ZERO ".",
            tid);
    else if (ret == MLE)
        End(std::string(YELLOW) + cmd[0] + RED " Memory limit eXCeeded" ZERO
                " on testcase " LBLUE + std::to_string(finished_count()) + ZERO ".",
            tid);
    else if (ret)
        End(std::string(YELLOW) + cmd[0] + RED " RE" ZERO
                " on testcase " LBLUE + std::to_string(finished_count()) + ZERO ".",
            tid);
}

// 各工作线程阻塞 SIGINT，由主线程的 newsigint 统一处理
static void block_sigint() {
    sigset_t ss;
    sigemptyset(&ss);
    sigaddset(&ss, SIGINT);
    pthread_sigmask(SIG_BLOCK, &ss, nullptr);
}

static void duipai(int tid) {
    block_sigint();
    while (!killed) {
        run_one({"./rand"}, "/dev/stdin", "in-" + std::to_string(tid) + ".txt",
                Config::time_limit, Config::mem_limit, tid);
        run_one({"./wa"}, "in-" + std::to_string(tid) + ".txt",
                "wa-" + std::to_string(tid) + ".txt",
                Config::time_limit, Config::mem_limit, tid);
        run_one({"./std"}, "in-" + std::to_string(tid) + ".txt",
                "std-" + std::to_string(tid) + ".txt",
                Config::time_limit, Config::mem_limit, tid);
        if (exe({"/bin/diff", "-Z",
                 "wa-" + std::to_string(tid) + ".txt",
                 "std-" + std::to_string(tid) + ".txt"},
                "/dev/null", "/dev/null", "/dev/null")) {
            End(RED "Wrong Answer" ZERO " on testcase " LBLUE +
                    std::to_string(finished_count()) + ZERO ".",
                tid);
            break;
        }
        cnt[tid]++;
    }
}

static void spj_check(int tid) {
    block_sigint();
    while (!killed) {
        run_one({"./rand"}, "/dev/stdin", "in-" + std::to_string(tid) + ".txt",
                Config::time_limit, Config::mem_limit, tid);
        run_one({"./wa"}, "in-" + std::to_string(tid) + ".txt",
                "wa-" + std::to_string(tid) + ".txt",
                Config::time_limit, Config::mem_limit, tid);
        if (exe({"./spj",
                 "in-" + std::to_string(tid) + ".txt",
                 "wa-" + std::to_string(tid) + ".txt",
                 "wa-" + std::to_string(tid) + ".txt"},
                "/dev/null", "/dev/null", "/dev/null")) {
            End(RED "Wrong Answer" ZERO " on testcase " LBLUE +
                    std::to_string(finished_count()) + ZERO ".",
                tid);
            break;
        }
        cnt[tid]++;
    }
}

static void spj_cmp(int tid) {
    block_sigint();
    while (!killed) {
        run_one({"./rand"}, "/dev/stdin", "in-" + std::to_string(tid) + ".txt",
                Config::time_limit, Config::mem_limit, tid);
        run_one({"./wa"}, "in-" + std::to_string(tid) + ".txt",
                "wa-" + std::to_string(tid) + ".txt",
                Config::time_limit, Config::mem_limit, tid);
        run_one({"./std"}, "in-" + std::to_string(tid) + ".txt",
                "std-" + std::to_string(tid) + ".txt",
                Config::time_limit, Config::mem_limit, tid);
        if (exe({"./spj",
                 "in-" + std::to_string(tid) + ".txt",
                 "wa-" + std::to_string(tid) + ".txt",
                 "std-" + std::to_string(tid) + ".txt"},
                "/dev/null", "/dev/null", "/dev/null")) {
            End(RED "Wrong Answer" ZERO " on testcase " LBLUE +
                    std::to_string(finished_count()) + ZERO ".",
                tid);
            break;
        }
        cnt[tid]++;
    }
}

static void detect(int tid) {
    block_sigint();
    while (!killed) {
        run_one({"./rand"}, "/dev/stdin", "in-" + std::to_string(tid) + ".txt",
                Config::time_limit, Config::mem_limit, tid);
        run_one({"./wa"}, "in-" + std::to_string(tid) + ".txt",
                "wa-" + std::to_string(tid) + ".txt",
                Config::time_limit, Config::mem_limit, tid);
        cnt[tid]++;
    }
}

static void gen(int tid, int n) {
    block_sigint();
    for (int i = tid; i <= n; i += Config::threads) {
        if (killed) break;
        run_one({"./rand", std::to_string(i)}, "/dev/stdin",
                "./csd/" + std::to_string(i) + ".in", -1, -1, tid);
        if (killed) break;
        run_one({"./std"}, "./csd/" + std::to_string(i) + ".in",
                "./csd/" + std::to_string(i) + ".out", -1, -1, tid);
        if (killed) break;
        cnt[tid]++;
    }
}

void run(int mode) {
    init();
    thr.clear();
    for (int i = 1; i <= Config::threads; i++) {
        if (mode == DUIPAI) thr.emplace_back(duipai, i);
        else if (mode == SPJCMP) thr.emplace_back(spj_cmp, i);
        else if (mode == SPJCHECK) thr.emplace_back(spj_check, i);
        else if (mode == DETECT) thr.emplace_back(detect, i);
    }
    auto tim = std::chrono::steady_clock::now();
    while (!killed) {
        usleep(20000);
        int cur = finished_count();
        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - tim).count();
        clear();
        std::printf("Testcase " LBLUE "#%d" ZERO "\n", cur);
        std::printf("Speed: " PURPLE "%lld testcases/min" ZERO "\n",
                    ms == 0 ? 0 : (long long)cur * 60000 / ms);
    }
    for (auto& t : thr) t.join();
    killed = false;
    if (id) {
        std::rename(("in-" + std::to_string(id) + ".txt").c_str(), "./in.txt");
        std::rename(("wa-" + std::to_string(id) + ".txt").c_str(), "./wa.txt");
        std::rename(("std-" + std::to_string(id) + ".txt").c_str(), "./std.txt");
    }
    for (int i = 1; i <= Config::threads; i++) {
        std::remove(("in-" + std::to_string(i) + ".txt").c_str());
        std::remove(("wa-" + std::to_string(i) + ".txt").c_str());
        std::remove(("std-" + std::to_string(i) + ".txt").c_str());
    }
    clear();
    std::printf("%s\n\n", mess.c_str());
}

void gen_main(int n) {
    mkdir("csd", 0777);
    DIR* dir = opendir("./csd");
    if (dir != nullptr) {
        dirent* ptr;
        while ((ptr = readdir(dir)) != nullptr) {
            if (!std::strcmp(ptr->d_name, ".") || !std::strcmp(ptr->d_name, ".."))
                continue;
            std::remove((std::string("csd/") + ptr->d_name).c_str());
        }
        closedir(dir);
    }
    init();
    thr.clear();
    for (int i = 1; i <= Config::threads; i++)
        thr.emplace_back(gen, i, n);
    auto tim = std::chrono::steady_clock::now();
    while (!killed && finished_count() < n) {
        usleep(20000);
        int cur = finished_count();
        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - tim).count();
        clear();
        std::printf("Generated " LBLUE "#%d" ZERO " testcases\n", cur);
        std::printf("Speed: " PURPLE "%lld testcases/min" ZERO "\n",
                    ms == 0 ? 0 : (long long)cur * 60000 / ms);
    }
    for (auto& t : thr) t.join();
    killed = false;
    // 检查 zip 是否可用，避免未安装时静默失败
    if (std::system("command -v zip > /dev/null 2>&1") == 0) {
        if (exe({"/bin/sh", "zip.sh"})) throw XCException("zip failed");
    } else {
        std::printf(YELLOW "Warning: zip not installed, skip packing.\n" ZERO);
    }
    for (int i = 1; i <= n; i++) {
        std::remove(("csd/" + std::to_string(i) + ".in").c_str());
        std::remove(("csd/" + std::to_string(i) + ".out").c_str());
    }
}

}  // namespace RUN
