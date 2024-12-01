#ifndef _XC_DUIPAI_COMMON_H_
#define _XC_DUIPAI_COMMON_H_

#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#define RED "\033[31;1m"
#define GREEN "\033[32;1m"
#define YELLOW "\033[33;1m"
#define DBLUE "\033[34;1m"
#define PURPLE "\033[35;1m"
#define LBLUE "\033[36;1m"
#define ZERO "\033[0m"
#define TLE 2236
#define MLE 2237
#define DUIPAI 2238
#define SPJCHECK 2239
#define DETECT 2240
#define GEN 2241
#define SPJCMP 2242
using namespace std;
using cmdlist = vector<string>;

inline void clear(){
    printf("\033[H\033[2J\033[3J");
}

inline int us(timeval tv){
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

bool killed = false;

void sigint(int){
    killed = true;
}

int exe(cmdlist cmd,string in = "/dev/stdin",string out = "/dev/stdout",string err = "/dev/stderr",int tl = -1,int ml = -1){
    if (access(cmd[0].c_str(),X_OK)){
        printf("=== \033[31;1mError:\033[0m cannot execute\033[33;1m %s\033[0m : no such file or directory!\n",cmd[0].c_str());
        throw "XC";
    }
    pid_t pid = fork();
    if (pid == -1){
        perror("=== \033[31;1mError:\033[33;1m fork()\033[0m failed:");
        throw "XC";
    }
    if (pid == 0){
        char **argv = new char*[cmd.size() + 1];
        for (int i = 0;i < (int)cmd.size();i ++){
            argv[i] = new char[cmd[i].length() + 1];
            strcpy(argv[i],cmd[i].c_str());
        }
        argv[cmd.size()] = NULL;
        freopen(in.c_str(),"r",stdin);
        freopen(out.c_str(),"w",stdout);
        freopen(err.c_str(),"w",stderr);
        rlimit rl;
        if (tl != -1){
            rl.rlim_cur = rl.rlim_max = (tl + 1999) / 1000;
            setrlimit(RLIMIT_CPU,&rl);
        }
        if (ml != -1){
            rl.rlim_cur = rl.rlim_max = (ml + 16) << 20;
            setrlimit(RLIMIT_AS,&rl);
        }
        execv(cmd[0].c_str(),argv);
        perror("=== \033[31;1mError:\033[33;1m execv()\033[0m failed:");
        return -1;
    }
    rusage ru;
    int status = 2236,ret;
    while (1){
        ret = wait4(pid,&status,WNOHANG,&ru);
        if (ret)break;
        if (killed)
            return -1;
        usleep(1);
    }
    if (tl != -1 && us(ru.ru_utime) + us(ru.ru_stime) > tl * 1000)
        return TLE;
    if (ml != -1 && ru.ru_maxrss > (ml << 10))
        return MLE;
    if (WIFSIGNALED(status))return 1;
    return WEXITSTATUS(status);
}

inline void clean_garbage_files(){
    exe({"/bin/rm","wa-*.txt","in-*.txt","std-*.txt"},"/dev/stdin","/dev/stdout","/dev/null");
}

#endif