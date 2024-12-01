#ifndef _XC_DUIPAI_RUN_H_
#define _XC_DUIPAI_RUN_H_

#include "common.h"
#include "config.h"

namespace RUN{
    mutex finish;
    thread thr[4096];
    int cnt[4096];
    string mess;
    int id;

    inline void newsigint(int);

    inline void init(){
        // clear();
        killed = false;
        signal(SIGINT,newsigint);
        memset(cnt,0,sizeof(cnt));
    }

    int count(){
        int ans = 0;
        for (int i = 1;i <= Config::threads;i ++)
            ans += cnt[i];
        return ans;
    }

    inline void End(string message,int id){
        finish.lock();
        if (killed){
            finish.unlock();
            return;
        }
        killed = true;
        finish.unlock();
        mess = message;
        RUN::id = id;
    }

    inline void run(vector<string> cmd,string in,string out,int tl,int ml,int id){
        int ret = exe(cmd,in,out,"/dev/stderr",tl,ml);
        if (ret == TLE)End("\033[33;1m" + cmd[0] + "\033[31;1m Time limit eXCeeded\033[0m on testcase \033[36;1m" + to_string(count()) + "\033[0m.",id);
        else if (ret == MLE)End("\033[33;1m" + cmd[0] + "\033[31;1m Memory limit eXCeeded\033[0m on testcase \033[36;1m" + to_string(count()) + "\033[0m.",id);
        else if (ret)End("\033[33;1m" + cmd[0] + "\033[31;1m RE\033[0m on testcase \033[36;1m" + to_string(count()) + "\033[0m.",id);
    }
    
    void newsigint(int){
        End(RED"Interrupted."ZERO,0);
    }

    void duipai(int id){
        sigset_t ss;
        sigemptyset(&ss);
        sigaddset(&ss, SIGINT);
        pthread_sigmask(SIG_BLOCK,&ss,NULL);
        while (!killed){
            run({"./rand"},"/dev/stdin","in-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            run({"./wa"},"in-" + to_string(id) + ".txt","wa-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            run({"./std"},"in-" + to_string(id) + ".txt","std-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            if (exe({"/bin/diff","-Z","wa-" + to_string(id) + ".txt","std-" + to_string(id) + ".txt"},"/dev/null","/dev/null","/dev/null")){
                End("\033[31;1mWrong Answer\033[0m on testcase \033[36;1m" + to_string(count()) + "\033[0m.",id);
                break;
            }
            cnt[id] ++;
        }
    }

    void spj_check(int id){
        sigset_t ss;
        sigemptyset(&ss);
        sigaddset(&ss, SIGINT);
        pthread_sigmask(SIG_BLOCK,&ss,NULL);
        while (!killed){
            run({"./rand"},"/dev/stdin","in-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            run({"./wa"},"in-" + to_string(id) + ".txt","wa-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            if (exe({"./spj","in-" + to_string(id) + ".txt","wa-" + to_string(id) + ".txt","wa-" + to_string(id) + ".txt"},"/dev/null","/dev/null","/dev/null")){
                End("\033[31;1mWrong Answer\033[0m on testcase \033[36;1m" + to_string(count()) + "\033[0m.",id);
                break;
            }
            cnt[id] ++;
        }
    }

    void spj_cmp(int id){
        sigset_t ss;
        sigemptyset(&ss);
        sigaddset(&ss, SIGINT);
        pthread_sigmask(SIG_BLOCK,&ss,NULL);
        while (!killed){
            run({"./rand"},"/dev/stdin","in-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            run({"./wa"},"in-" + to_string(id) + ".txt","wa-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            run({"./std"},"in-" + to_string(id) + ".txt","std-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            if (exe({"./spj","in-" + to_string(id) + ".txt","wa-" + to_string(id) + ".txt","std-" + to_string(id) + ".txt"},"/dev/null","/dev/null","/dev/null")){
                End("\033[31;1mWrong Answer\033[0m on testcase \033[36;1m" + to_string(count()) + "\033[0m.",id);
                break;
            }
            cnt[id] ++;
        }
    }

    void detect(int id){
        sigset_t ss;
        sigemptyset(&ss);
        sigaddset(&ss, SIGINT);
        pthread_sigmask(SIG_BLOCK,&ss,NULL);
        while (!killed){
            run({"./rand"},"/dev/stdin","in-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            run({"./wa"},"in-" + to_string(id) + ".txt","wa-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            cnt[id] ++;
        }
    }

    void gen(int id,int n){
        sigset_t ss;
        sigemptyset(&ss);
        sigaddset(&ss, SIGINT);
        pthread_sigmask(SIG_BLOCK,&ss,NULL);
        for (int i = id;i <= n;i += Config::threads){
            if (killed)break;
            run({"./rand",to_string(i)},"/dev/stdin","./csd/" + to_string(i) + ".in",-1,-1,id);
            if (killed)break;
            run({"./std"},"./csd/" + to_string(i) + ".in","./csd/" + to_string(i) + ".out",-1,-1,id);
            if (killed)break;
            cnt[id] ++;
        }
    }

    void run(int mode){
        init();
        for (int i = 1;i <= Config::threads;i ++){
            if (mode == DUIPAI)thr[i] = thread(duipai,i);
            else if (mode == SPJCMP)thr[i] = thread(spj_cmp,i);
            else if (mode == SPJCHECK)thr[i] = thread(spj_check,i);
            else if (mode == DETECT)thr[i] = thread(detect,i);
        }
        auto tim = chrono::high_resolution_clock::now();
        while (!killed){
            usleep(20000);
            int cur = count();
            auto now = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<std::chrono::milliseconds>(now - tim);
            long long val = duration.count();
            clear();
            printf("Testcase \033[36;1m#%d\033[0m\n",cur);
            printf("Speed: "PURPLE"%lld testcases/min"ZERO"\n",!val ? 0 : (long long)cur * 60000 / val);
        }
        for (int i = 1;i <= Config::threads;i ++)
            thr[i].join();
        killed = false;
        if (id){
            rename(("in-" + to_string(id) + ".txt").c_str(),"./in.txt");
            rename(("wa-" + to_string(id) + ".txt").c_str(),"./wa.txt");
            rename(("std-" + to_string(id) + ".txt").c_str(),"./std.txt");
        }
        for (int i = 1;i <= Config::threads;i ++){
            remove(("in-" + to_string(i) + ".txt").c_str());
            remove(("wa-" + to_string(i) + ".txt").c_str());
            remove(("std-" + to_string(i) + ".txt").c_str());
        }
        clear();
        printf("%s\n\n",mess.c_str());
    }

    void gen_main(int n){
        mkdir("csd", 0777);
        DIR *dir = opendir("./csd");
        if(dir != NULL){
            dirent *ptr;
            while ((ptr = readdir(dir)) != NULL){
                if (!strcmp(ptr->d_name,".") || !strcmp(ptr->d_name,".."))continue;
                remove(((string)"csd/" + ptr->d_name).c_str());
            }
            closedir(dir);
        }
        remove("csd/testcases.zip");
        init();
        for (int i = 1;i <= Config::threads;i ++)
            thr[i] = thread(gen,i,n);
        int lst = 0;
        clock_t tim = clock();
        while (!killed && count() < n){
            usleep(20000);
            int cur = count();
            clock_t now = clock();
            clear();
            printf("Generated \033[36;1m#%d\033[0m testcases\n",cur);
            printf("Speed: "PURPLE"%lld testcases/min"ZERO"\n",(now == tim) ? 0 : (long long)(cur - lst) * CLOCKS_PER_SEC * 60 / (now - tim));
        }
        for (int i = 1;i <= Config::threads;i ++)
            thr[i].join();
        killed = false;
        if (exe({"/bin/sh","zip.sh"}))throw "XC";
        for (int i = 1;i <= n;i ++)
            remove(("csd/" + to_string(i) + ".in").c_str()),remove(("csd/" + to_string(i) + ".out").c_str());
    }
}


#endif