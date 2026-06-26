#ifndef XC_DUIPAI_CONFIG_H
#define XC_DUIPAI_CONFIG_H

namespace Config {
// 默认值：threads=4, time_limit=-1(无限制), mem_limit=-1(无限制)
extern int time_limit;
extern int mem_limit;
extern int threads;

// 从 config 文件读取；文件不存在或解析失败时用默认值并落盘
void read();
// 保存到 config 文件
void save();
}  // namespace Config

#endif
