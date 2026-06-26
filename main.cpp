#include "src/commands.h"
#include "src/common.h"
#include "src/complete.h"
#include "src/config.h"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>

#include <readline/readline.h>
#include <readline/history.h>

int main() {
    std::printf(
        "Thanks for choosing \033[35;1mXC-duipai\033[0m powered by\033[33;1m WCK\033[0m!\n\n"
        "\033[35;1mXC-duipai\033[0m supports:                   \033[33;1m*     *     ******\033[0m\n"
        "  - \033[36;1mduipai\033[0m                            \033[33;1m *   *     *       \033[0m\n"
        "  - \033[36;1mspj-duipai\033[0m                        \033[33;1m  * *      *       \033[0m\n"
        "  - \033[36;1mtestcase-generating\033[0m               \033[33;1m   *       *       \033[0m\n"
        "  - \033[36;1mmulti-threading\033[0m                   \033[33;1m  * *      *       \033[0m\n"
        "  - \033[36;1mTLE/MLE/RE\033[0m detecting              \033[33;1m *   *     *       \033[0m\n"
        "  - The\033[36;1m readline\033[0m library              \033[33;1m*     *     *******\033[0m\n"
        "\n");

    rl_attempted_completion_function = Complete::fileman_completion;
    Config::read();

    while (true) {
        char* s = readline("\001\033[01;34m\002XC$\001\033[0m\002 ");
        if (!s) break;
        // RAII 管理 readline 返回的内存，避免异常路径泄漏
        std::unique_ptr<char, decltype(&std::free)> s_guard(s, &std::free);

        cmdlist cmd;
        std::stringstream ss(s);
        std::string tmp;
        while (ss >> tmp) cmd.push_back(tmp);
        if (cmd.empty()) continue;

        try {
            add_history(s);
            signal(SIGINT, sigint);
            if (!commands::dispatch(cmd)) {
                // 非内置命令，交给 shell 执行
                std::string sh_cmd = cmd[0];
                for (size_t i = 1; i < cmd.size(); i++) sh_cmd += " " + cmd[i];
                exe({"/bin/sh", "-c", sh_cmd});
            }
        } catch (const QuitException&) {
            clean_garbage_files();
            return 0;
        } catch (const XCException&) {
            // 命令执行出错，错误信息已打印，继续下一轮
        } catch (...) {
            // 未知异常，静默继续
        }
        killed = false;
        signal(SIGINT, SIG_DFL);
        clean_garbage_files();
    }
    clean_garbage_files();
    return 0;
}
