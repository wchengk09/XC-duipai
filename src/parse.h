#ifndef _XC_DUIPAI_PARSE_H_
#define _XC_DUIPAI_PARSE_H_

#include "common.h"

namespace Parse{
    bool number(const char *str) {
        bool flag = false;
        while (*str) {
            if (*str != '-' && !isdigit(*str))
                return false;
            flag = true;
            str++;
        }
        return flag;
    }

    inline void least(const cmdlist &cmd,int id){
        if ((int)cmd.size() <= id){
            printf("\033[31;1mError:\033[0m command \033[33;1m%s\033[0m requires at least \033[35;1m%d\033[0m arguments!\n",cmd[0].c_str(),id);
            throw "XC";
        }
    }

    inline int number(const cmdlist &cmd,int id){
        least(cmd,id);
        if (!number(cmd[id].c_str())){
            printf("\033[31;1mError:\033[36m %s\033[0m is not a number!\n",cmd[id].c_str());
            throw "XC";
        }
        return atoi(cmd[id].c_str());
    }

    inline int range(const cmdlist &cmd,int id,int l,int r){
        int x = number(cmd,id);
        if (x < l || x > r){
            printf("\033[31;1mError:\033[36m %d\033[0m is not in the range \033[35;1m[%d,%d]\033[0m!\n",x,l,r);
            throw "XC";
        }
        return x;
    }
}

#endif