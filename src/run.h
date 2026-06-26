#ifndef XC_DUIPAI_RUN_H
#define XC_DUIPAI_RUN_H

namespace RUN {
// 启动对拍/检测（DUIPAI / SPJCHECK / SPJCMP / DETECT）
void run(int mode);
// 生成 n 组测试数据并打包
void gen_main(int n);
}  // namespace RUN

#endif
