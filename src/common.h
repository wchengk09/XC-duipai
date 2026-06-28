#ifndef XC_DUIPAI_COMMON_H
#define XC_DUIPAI_COMMON_H

#include <string>
#include <vector>
#include <sys/time.h>
#include <sys/resource.h>

// ANSI 颜色码：用于字符串字面量拼接，保留为宏
#define RED    "\033[31;1m"
#define GREEN  "\033[32;1m"
#define YELLOW "\033[33;1m"
#define DBLUE  "\033[34;1m"
#define PURPLE "\033[35;1m"
#define LBLUE  "\033[36;1m"
#define ZERO   "\033[0m"

// 返回码：使用较大值避免与程序正常退出码冲突
inline constexpr int TLE = 2236;  // Time Limit Exceeded
inline constexpr int MLE = 2237;  // Memory Limit Exceeded

// 运行模式
inline constexpr int DUIPAI   = 2238;  // 普通对拍
inline constexpr int SPJCHECK = 2239;  // SPJ 检测
inline constexpr int DETECT   = 2240;  // TLE/MLE/RE 检测
inline constexpr int GEN      = 2241;  // 数据生成
inline constexpr int SPJCMP   = 2242;  // SPJ 比较

// 线程数上限，与 Parse::range 的上界保持同步
inline constexpr int MAX_THREADS = 4096;

// 命令列表类型
using cmdlist = std::vector<std::string>;

// 命令执行出错时抛出（替代裸字符串 throw）
struct XCException {
    const char* msg;
    explicit XCException(const char* m = "") : msg(m) {}
};

// quit / exit 时抛出，由主循环捕获
struct QuitException {};

// 用户中断标志（由 SIGINT 处理函数设置）
extern bool killed;

// 清屏
void clear();
// timeval 转微秒
long long to_us(const timeval& tv);
// SIGINT 处理函数
void sigint(int);
// 执行外部命令，返回退出码或 TLE/MLE；出错抛 XCException
int exe(const cmdlist& cmd, const std::string& in = "/dev/stdin",
        const std::string& out = "/dev/stdout",
        const std::string& err = "/dev/stderr",
        int tl = -1, int ml = -1);
// 等效替代 diff -Z：忽略行尾空白后比较两文件，返回 0 相同 / 1 不同（与 diff 返回码一致）
// 进程内实现，避免每次 fork /bin/diff 的开销
int diff_Z(const std::string& f1, const std::string& f2);
// 对拍临时文件目录前缀（含末尾 /）：优先 /dev/shm/xc-duipai-<pid>/（RAM-backed），
// 不可用则返回空串（回退到 CWD）。首次调用线程安全地创建子目录。
const std::string& tmpprefix();
// 拷贝文件（跨文件系统安全，用于把 WA 样例从 /dev/shm 拷回 CWD）
bool copy_file(const std::string& from, const std::string& to);
// 清理对拍临时文件（wa-*.txt / in-*.txt / std-*.txt）
void clean_garbage_files();

#endif
