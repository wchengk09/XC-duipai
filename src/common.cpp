#include "common.h"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
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

        if (in != "/dev/stdin") {
            int fd = ::open(in.c_str(), O_RDONLY);
            if (fd < 0) { std::perror("open stdin"); std::_Exit(1); }
            ::dup2(fd, STDIN_FILENO);
            ::close(fd);
        }
        if (out != "/dev/stdout") {
            int fd = ::open(out.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { std::perror("open stdout"); std::_Exit(1); }
            ::dup2(fd, STDOUT_FILENO);
            ::close(fd);
        }
        if (err != "/dev/stderr") {
            int fd = ::open(err.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { std::perror("open stderr"); std::_Exit(1); }
            ::dup2(fd, STDERR_FILENO);
            ::close(fd);
        }
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

// 等效替代 diff -Z：逐行比较，忽略每行行尾空白（space/tab/\r/\v/\f 等 isspace 字符）
// 与 /bin/diff -Z 行为一致：行数不同→不同；行尾换行有无→相同；纯空白行→等价于空行
int diff_Z(const std::string& f1, const std::string& f2) {
    std::ifstream a(f1), b(f2);
    if (!a.is_open() || !b.is_open()) return 1;  // 打开失败视为不同，安全侧
    std::string la, lb;
    // 去掉行尾空白，与 diff -Z 语义一致
    auto strip = [](std::string& s) {
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
            s.pop_back();
    };
    while (true) {
        bool ea = !std::getline(a, la);
        bool eb = !std::getline(b, lb);
        if (ea != eb) return 1;  // 行数不同
        if (ea) return 0;        // 同时读完，全部相同
        strip(la);
        strip(lb);
        if (la != lb) return 1;
    }
}

// 对拍临时文件目录前缀：优先 /dev/shm/xc-duipai-<pid>/，回退空串（CWD）
// 用 C++11 magic statics 保证线程安全：多线程并发首次调用只会初始化一次
const std::string& tmpprefix() {
    static const std::string p = []() -> std::string {
        if (access("/dev/shm", W_OK | X_OK) == 0) {
            std::string d = "/dev/shm/xc-duipai-" + std::to_string(getpid());
            if (mkdir(d.c_str(), 0700) == 0 || errno == EEXIST)
                return d + "/";
        }
        return "";  // 回退到 CWD
    }();
    return p;
}

// 跨文件系统安全的文件拷贝（std::rename 在跨 fs 时返回 EXDEV）
bool copy_file(const std::string& from, const std::string& to) {
    std::ifstream i(from, std::ios::binary);
    if (!i) return false;
    std::ofstream o(to, std::ios::binary);
    if (!o) return false;
    o << i.rdbuf();
    return static_cast<bool>(o);
}

void clean_garbage_files() {
    const std::string& p = tmpprefix();
    if (!p.empty()) {
        // /dev/shm 子目录：用 syscall 枚举并删除所有文件，再 rmdir
        // 避免 sh -c rm 被 sandbox 拦截，且省去 fork/exec 开销
        DIR* d = opendir(p.c_str());
        if (d) {
            struct dirent* e;
            while ((e = readdir(d)) != nullptr) {
                if (e->d_name[0] == '.') continue;
                std::remove((p + e->d_name).c_str());
            }
            closedir(d);
        }
        // 不 rmdir — tmpprefix() 缓存的路径会被后续命令误用，导致 open() 失败→RE
    } else {
        // 回退到 CWD：经 sh -c 展开通配符
        exe({"/bin/sh", "-c", "rm -f wa-*.txt in-*.txt std-*.txt 2>/dev/null"},
            "/dev/stdin", "/dev/stdout", "/dev/null");
    }
}
