#ifndef XC_DUIPAI_COMPLETE_H
#define XC_DUIPAI_COMPLETE_H

#include <cstdio>
#include <readline/readline.h>

namespace Complete {
// readline 补全回调：对首个单词补全内置命令
char** fileman_completion(const char* text, int start, int);
}  // namespace Complete

#endif
