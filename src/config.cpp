#include "config.h"

#include <cstdio>

namespace Config {

int time_limit = -1;
int mem_limit = -1;
int threads = 4;

void read() {
    FILE* fp = std::fopen("config", "r");
    if (!fp) {
        save();  // 配置不存在则用默认值并创建
        return;
    }
    if (std::fscanf(fp, "%d%d%d", &threads, &time_limit, &mem_limit) != 3) {
        threads = 4;
        time_limit = -1;
        mem_limit = -1;  // 解析失败则重置默认
    }
    std::fclose(fp);
}

void save() {
    FILE* fp = std::fopen("config", "w");
    if (!fp) return;
    std::fprintf(fp, "%d %d %d\n", threads, time_limit, mem_limit);
    std::fclose(fp);
}

}  // namespace Config
