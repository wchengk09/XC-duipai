#include "run.h"

#include "common.h"
#include "config.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <set>
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

// ---- hack 模式专用状态 ----
static std::mutex hack_mutex;
static std::vector<std::string> hack_names;   // hacks/ 下的解法名（不含扩展名）
static int hack_found = 0;                    // 已找到的 hack 测试点数
static int hack_attempts = 0;                 // 已尝试的种子数
static int attempts_since_last_save = 0;      // 距上次保存测试点以来的尝试次数（-m 用）
static std::set<std::string> hacked_solutions; // -o 模式下已被 hack 的解法集合

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

// =============== hack 模式实现 ===============

// 列出 hacks/ 目录下所有 .cpp 文件名（去后缀），按字典序排序
std::vector<std::string> list_hacks() {
    std::vector<std::string> names;
    DIR* dir = opendir("./hacks");
    if (!dir) return names;
    dirent* ptr;
    while ((ptr = readdir(dir)) != nullptr) {
        std::string name = ptr->d_name;
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".cpp") == 0) {
            names.push_back(name.substr(0, name.size() - 4));
        }
    }
    closedir(dir);
    std::sort(names.begin(), names.end());
    return names;
}

// 尝试一个种子：生成数据 → 跑 std → 跑每个 hack → 判断是否 hack 成功
// 成功则把数据保存为 csd/<hack_found>.in/.out，失败则删除临时文件
// max_attempts>=0 时，连续失败 max_attempts 次后无视 hack 规则直接保存
static bool hack_try(int tid, int seed, int n, bool all_mode, bool cover_mode,
                     int max_attempts) {
    std::string in  = "./csd/hack-" + std::to_string(tid) + ".in";
    std::string out = "./csd/hack-" + std::to_string(tid) + ".out";

    // rand 与 std 不设时限/空限，保证数据本身正确
    int ret = exe({"./rand", std::to_string(seed)}, "/dev/stdin", in, "/dev/null", -1, -1);
    if (ret) {
        End(std::string("./rand") + RED " RE" ZERO " on seed " + std::to_string(seed), tid);
        return false;
    }
    if (killed) return false;

    ret = exe({"./std"}, in, out, "/dev/null", -1, -1);
    if (ret) {
        End(std::string("./std") + RED " RE" ZERO " on seed " + std::to_string(seed), tid);
        return false;
    }
    if (killed) return false;

    // 预检查：是否已达 max_attempts 上限，若是则跳过 hack 直接保存
    {
        std::lock_guard<std::mutex> lk(hack_mutex);
        hack_attempts++;
        attempts_since_last_save++;
        if (max_attempts >= 0 && attempts_since_last_save > max_attempts &&
            hack_found < n) {
            // 连续失败超过 max_attempts 次，无视 hack 规则直接保存
            hack_found++;
            attempts_since_last_save = 0;
            std::rename(in.c_str(),
                        ("./csd/" + std::to_string(hack_found) + ".in").c_str());
            std::rename(out.c_str(),
                        ("./csd/" + std::to_string(hack_found) + ".out").c_str());
            return true;
        }
    }
    if (killed) return false;

    // 跑每个 hack 解法，记录哪些被 hack 掉
    int failed = 0;
    std::vector<std::string> hacked_now;  // 本次被 hack 的解法名
    for (const auto& name : hack_names) {
        std::string hack_out = "./csd/hack-" + std::to_string(tid) + "." + name;
        ret = exe({"./hacks/" + name}, in, hack_out, "/dev/null",
                  Config::time_limit, Config::mem_limit);
        bool hacked;
        if (ret == TLE || ret == MLE || ret != 0) {
            // TLE / MLE / RE 视为 hack 成功
            hacked = true;
        } else {
            // 与 std 输出做 diff（忽略行尾空格）
            hacked = exe({"/bin/diff", "-Z", hack_out, out},
                         "/dev/null", "/dev/null", "/dev/null") != 0;
        }
        if (hacked) {
            failed++;
            hacked_now.push_back(name);
        }
        std::remove(hack_out.c_str());
    }

    bool success = all_mode ? (failed == (int)hack_names.size()) : (failed >= 1);

    {
        std::lock_guard<std::mutex> lk(hack_mutex);
        // 判断是否保存该测试点（计数已在预检查中递增）
        bool should_save = false;
        if (success && hack_found < n) {
            if (cover_mode) {
                // -o 模式：必须 hack 至少一份之前从未被 hack 过的代码
                for (const auto& name : hacked_now) {
                    if (hacked_solutions.find(name) == hacked_solutions.end()) {
                        should_save = true;
                        break;
                    }
                }
            } else {
                should_save = true;
            }
        }
        if (should_save) {
            hack_found++;
            attempts_since_last_save = 0;
            std::rename(in.c_str(),
                        ("./csd/" + std::to_string(hack_found) + ".in").c_str());
            std::rename(out.c_str(),
                        ("./csd/" + std::to_string(hack_found) + ".out").c_str());
            for (const auto& name : hacked_now)
                hacked_solutions.insert(name);
        } else {
            std::remove(in.c_str());
            std::remove(out.c_str());
        }
    }
    return success;
}

static void hack_worker(int tid, int n, bool all_mode, bool cover_mode,
                        int max_attempts) {
    block_sigint();
    for (int seed = tid; ; seed += Config::threads) {
        if (killed) break;
        {
            std::lock_guard<std::mutex> lk(hack_mutex);
            if (hack_found >= n) break;
        }
        hack_try(tid, seed, n, all_mode, cover_mode, max_attempts);
    }
}

void hack_main(int n, bool all_mode, bool cover_mode, int max_attempts) {
    // 读取 hacks/ 下的解法列表，供工作线程使用
    hack_names = list_hacks();
    if (hack_names.empty()) {
        std::printf(RED "Error:" ZERO " no hack solutions found in " YELLOW "hacks/" ZERO
                    " directory.\n");
        throw XCException("no hacks");
    }

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
    hack_found = 0;
    hack_attempts = 0;
    attempts_since_last_save = 0;
    hacked_solutions.clear();

    thr.clear();
    for (int i = 1; i <= Config::threads; i++)
        thr.emplace_back(hack_worker, i, n, all_mode, cover_mode, max_attempts);

    auto tim = std::chrono::steady_clock::now();
    while (true) {
        {
            std::lock_guard<std::mutex> lk(hack_mutex);
            if (hack_found >= n) break;
        }
        if (killed) break;
        usleep(20000);
        int cur_found, cur_attempts, cur_since, cur_covered;
        {
            std::lock_guard<std::mutex> lk(hack_mutex);
            cur_found = hack_found;
            cur_attempts = hack_attempts;
            cur_since = attempts_since_last_save;
            cur_covered = hacked_solutions.size();
        }
        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - tim).count();
        clear();
        std::printf("Hacked " LBLUE "%d/%d" ZERO " testcases\n", cur_found, n);
        if (cover_mode) {
            std::printf("Coverage: " PURPLE "%d/%d" ZERO " solutions hacked\n",
                        cur_covered, (int)hack_names.size());
        }
        std::printf("Attempts: " PURPLE "%d" ZERO, cur_attempts);
        if (max_attempts >= 0)
            std::printf("  (since last save: " YELLOW "%d/%d" ZERO ")",
                        cur_since, max_attempts);
        std::printf("\n");
        std::printf("Speed: " PURPLE "%lld attempts/min" ZERO "\n",
                    ms == 0 ? 0 : (long long)cur_attempts * 60000 / ms);
    }

    for (auto& t : thr) t.join();
    killed = false;

    // 清理线程私有临时文件
    for (int i = 1; i <= Config::threads; i++) {
        std::remove(("./csd/hack-" + std::to_string(i) + ".in").c_str());
        std::remove(("./csd/hack-" + std::to_string(i) + ".out").c_str());
        for (const auto& name : hack_names) {
            std::remove(("./csd/hack-" + std::to_string(i) + "." + name).c_str());
        }
    }

    clear();
    if (!mess.empty()) {
        // rand/std 出错或 Ctrl+C，直接打印错误信息
        std::printf("%s\n\n", mess.c_str());
    } else if (hack_found == 0) {
        std::printf(RED "No hack testcase found in %d attempts.\n" ZERO, hack_attempts);
        std::printf(YELLOW "Try improving your data generator (rand.cpp)"
                    " or use a different hack policy.\n" ZERO);
    } else {
        if (hack_found < n) {
            std::printf(YELLOW "Only found " LBLUE "%d" YELLOW " / %d hack testcases"
                        " in %d attempts.\n" ZERO, hack_found, n, hack_attempts);
        } else {
            std::printf(GREEN "Found " LBLUE "%d" GREEN " hack testcase(s) in "
                        PURPLE "%d" GREEN " attempts.\n" ZERO,
                        hack_found, hack_attempts);
        }
        // -o 模式下检查覆盖情况
        if (cover_mode && (int)hacked_solutions.size() < (int)hack_names.size()) {
            std::printf(YELLOW "\nWarning: " ZERO "以下解法未被 hack：\n");
            for (const auto& name : hack_names) {
                if (hacked_solutions.find(name) == hacked_solutions.end())
                    std::printf("  " RED "%s" ZERO "\n", name.c_str());
            }
        }
        // 打包
        if (std::system("command -v zip > /dev/null 2>&1") == 0) {
            if (exe({"/bin/sh", "zip.sh"})) throw XCException("zip failed");
        } else {
            std::printf(YELLOW "Warning: zip not installed, skip packing.\n" ZERO);
        }
    }

    // 清理已生成的测试点文件
    for (int i = 1; i <= hack_found; i++) {
        std::remove(("csd/" + std::to_string(i) + ".in").c_str());
        std::remove(("csd/" + std::to_string(i) + ".out").c_str());
    }
    std::printf("\n");
}

}  // namespace RUN
