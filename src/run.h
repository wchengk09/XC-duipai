#ifndef XC_DUIPAI_RUN_H
#define XC_DUIPAI_RUN_H

#include <string>
#include <vector>

namespace RUN {
// 启动对拍/检测（DUIPAI / SPJCHECK / SPJCMP / DETECT）
void run(int mode);
// 生成 n 组测试数据并打包
void gen_main(int n);
// 找出 n 组能 hack 掉 hacks/ 中解法的测试数据并打包
// all_mode=true 表示必须 hack 掉所有解法；false 表示 hack 掉至少一个即可
// cover_mode=true (-o) 表示要求每个 hack 解法至少被 hack 一次
// max_attempts>=0 (-m) 表示连续失败 max_attempts 次后无视 hack 规则直接保存；-1 表示无限制
void hack_main(int n, bool all_mode, bool cover_mode, int max_attempts);
// 列出 hacks/ 目录下所有 hack 解法的名字（去掉 .cpp 后缀）
std::vector<std::string> list_hacks();
}  // namespace RUN

#endif
