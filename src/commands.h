#ifndef XC_DUIPAI_COMMANDS_H
#define XC_DUIPAI_COMMANDS_H

#include "common.h"

namespace commands {
// 分发内置命令。返回 true 表示已处理；返回 false 表示非内置命令，
// 应由调用者作为 shell 命令执行。
bool dispatch(const cmdlist& cmd);
}  // namespace commands

#endif
