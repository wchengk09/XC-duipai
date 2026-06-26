#ifndef XC_DUIPAI_PARSE_H
#define XC_DUIPAI_PARSE_H

#include "common.h"

#include <string>

namespace Parse {
// 判断字符串是否为合法整数
bool is_number(const std::string& s);
// 检查命令至少有 id 个参数，否则抛 XCException
void least(const cmdlist& cmd, int id);
// 解析第 id 个参数为整数
int number(const cmdlist& cmd, int id);
// 解析第 id 个参数为整数并校验范围 [l, r]
int range(const cmdlist& cmd, int id, int l, int r);
}  // namespace Parse

#endif
