#include "common.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

bool killed = false;

void clear() {
    std::printf("\033[H\033[2J\033[3J");
}

long long to_us(const timeval& tv) {
    return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

void sigint(int) {
    killed = true;
}

int exe(const cmdlist& cmd, const std::string& in, const std::string& out,
        const std::string& err, int tl, int ml) {
    if (access(cmd[0].c_str(), X_OK)) {
        std::printf("=== " RED "Error:" ZERO " cannot execute" YELLOW " %s" ZERO
                    " : no such file or not executable!\n",
                    cmd[0].c_str());
        throw XCException("exec access");
    }
    // fork 前刷新所有 stdio 流，避免子进程继承未刷新的缓冲区导致输出重复
    std::fflush(nullptr);
    pid_t pid = fork();
    if (pid == -1) {
        std::perror("=== " RED "Error:" YELLOW " fork()" ZERO " failed:");
        throw XCException("fork");
    }
    if (pid == 0) {
        // 用 vector 管理 argv，避免内存泄漏
        std::vector<std::vector<char>> buf;
        std::vector<char*> argv;
        argv.reserve(cmd.size() + 1);
        for (const auto& s : cmd) {
            buf.emplace_back(s.begin(), s.end());
            buf.back().push_back('\0');
            argv.push_back(buf.back().data());
        }
        argv.push_back(nullptr);

        std::freopen(in.c_str(), "r", stdin);
        std::freopen(out.c_str(), "w", stdout);
        std::freopen(err.c_str(), "w", stderr);
        rlimit rl;
        if (tl != -1) {
            rl.rlim_cur = rl.rlim_max = (tl + 1999) / 1000;
            setrlimit(RLIMIT_CPU, &rl);
        }
        if (ml != -1) {
            // 额外预留 16MB 给运行时与栈
            rl.rlim_cur = rl.rlim_max = (ml + 16) << 20;
            setrlimit(RLIMIT_AS, &rl);
        }
        execv(cmd[0].c_str(), argv.data());
        std::perror("=== " RED "Error:" YELLOW " execv()" ZERO " failed:");
        std::_Exit(127);  // execv 失败必须立即终止子进程，不可返回
    }
    rusage ru;
    int status = 0, ret;
    while (1) {
        ret = wait4(pid, &status, WNOHANG, &ru);
        if (ret) break;
        if (killed) return -1;
        usleep(1000);  // 让出 CPU，避免忙等
    }
    if (tl != -1 && to_us(ru.ru_utime) + to_us(ru.ru_stime) > (long long)tl * 1000)
        return TLE;
    if (ml != -1 && ru.ru_maxrss > (ml << 10))
        return MLE;
    if (WIFSIGNALED(status)) return 1;
    return WEXITSTATUS(status);
}

void clean_garbage_files() {
    // 经 sh -c 执行以展开通配符（execv 不经 shell，无法扩展 *）
    exe({"/bin/sh", "-c", "rm -f wa-*.txt in-*.txt std-*.txt 2>/dev/null"},
        "/dev/stdin", "/dev/stdout", "/dev/null");
}
