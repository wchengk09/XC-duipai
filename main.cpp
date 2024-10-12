#include <bits/stdc++.h>
#include <readline/readline.h>
#include <readline/history.h>
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
#define SPJ 2239
#define DETECT 2240
#define GEN 2241
using namespace std;
using cmdlist = vector<string>;

inline void clear(){
    printf("\033[H\033[2J\033[3J");
}

namespace Complete{
    char *commands[] = {
        "run",
        "spj",
        "tle",
        "gen",
        "time",
        "mem",
        "threads",
        "getconf",
        "exit",
        "quit",
        "clear",
        "exe",
        NULL
    };

    inline char *dupstr(char *s){
        char *r;
        r = (char*)malloc(strlen(s) + 1);
        strcpy(r, s);
        return (r);
    }

    char *command_generator(const char *text, int state){
        static int list_index, len;
        char *name;

        if (!state){
            list_index = 0;
            len = strlen(text);
        }
    
        while ((name = commands[list_index])){
            list_index++;
            if (strncmp(name, text, len) == 0)
                return (dupstr(name));
        }
    
        return ((char *)NULL);
    }

    char **fileman_completion(const char *text,int start,int)
    {
        char **matches = (char **)NULL;

        if (start == 0)
            matches = rl_completion_matches(text, command_generator);
    
        return (matches);
    }
}

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

namespace Config{
    int time_limit = -1,mem_limit = -1,threads = 4;
    inline void read(){
        FILE *fp = fopen("config","r");
        fscanf(fp,"%d%d%d",&threads,&time_limit,&mem_limit);
        fclose(fp);
    }
    inline void save(){
        FILE *fp = fopen("config","w");
        fprintf(fp,"%d %d %d\n",threads,time_limit,mem_limit);
        fclose(fp);
    }
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

    void spj(int id){
        sigset_t ss;
        sigemptyset(&ss);
        sigaddset(&ss, SIGINT);
        pthread_sigmask(SIG_BLOCK,&ss,NULL);
        while (!killed){
            run({"./rand"},"/dev/stdin","in-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            run({"./wa"},"in-" + to_string(id) + ".txt","wa-" + to_string(id) + ".txt",Config::time_limit,Config::mem_limit,id);
            if (exe({"./std","in-" + to_string(id) + ".txt","wa-" + to_string(id) + ".txt"},"/dev/null","/dev/null","/dev/null")){
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
            else if (mode == SPJ)thr[i] = thread(spj,i);
            else if (mode == DETECT)thr[i] = thread(detect,i);
        }
        int lst = 0;
        clock_t tim = clock();
        while (!killed){
            usleep(20000);
            int cur = count();
            clock_t now = clock();
            clear();
            printf("Testcase \033[36;1m#%d\033[0m\n",cur);
            printf("Speed: "PURPLE"%lld testcases/min"ZERO"\n",(now == tim) ? 0 : (long long)(cur - lst) * CLOCKS_PER_SEC * 60 / (now - tim));
        }
        for (int i = 1;i <= Config::threads;i ++)
            thr[i].join();
        killed = false;
        if (id){
            rename(("in-" + to_string(id) + ".txt").c_str(),"in.txt");
            rename(("wa-" + to_string(id) + ".txt").c_str(),"wa.txt");
            rename(("std-" + to_string(id) + ".txt").c_str(),"std.txt");
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

int main(){
    printf(
        "Thanks for choosing \033[35;1mduipai-XC\033[0m powered by\033[33;1m WCK\033[0m!\n\n"
        "\033[35;1mduipai-XC\033[0m supports:                   \033[33;1m*     *     ******\033[0m\n"
        "  - \033[36;1mduipai\033[0m                            \033[33;1m *   *     *       \033[0m\n"
        "  - \033[36;1mspj-duipai\033[0m                        \033[33;1m  * *      *       \033[0m\n"
        "  - \033[36;1mtestcase-generating\033[0m               \033[33;1m   *       *       \033[0m\n"
        "  - \033[36;1mmulti-threading\033[0m                   \033[33;1m  * *      *       \033[0m\n"
        "  - \033[36;1mTLE/MLE/RE\033[0m detecting              \033[33;1m *   *     *       \033[0m\n"
        "  - The\033[36;1m readline\033[0m library              \033[33;1m*     *     *******\033[0m\n"
        "\n"
    );
    rl_attempted_completion_function = Complete::fileman_completion;
    Config::read();
    while (1){
        char *s = readline("\001\033[01;34m\002XC$\001\033[0m\002 ");
        if (!s)break;
        cmdlist cmd;
        stringstream ss(s);
        string tmp;
        while (ss >> tmp)
            cmd.push_back(tmp);
        if (cmd.empty())continue;
        try{
            add_history(s);
            signal(SIGINT,sigint);
            if (cmd[0] == "clear")
                clear();
            else if (cmd[0] == "threads"){
                Config::threads = Parse::range(cmd,1,1,4096);
                Config::save();
            }
            else if (cmd[0] == "time"){
                Config::time_limit = Parse::range(cmd,1,-1,INT_MAX / 1000);
                Config::save();
            }
            else if (cmd[0] == "mem"){
                Config::mem_limit = Parse::range(cmd,1,-1,INT_MAX >> 20);
                Config::save();
            }
            else if (cmd[0] == "getconf"){
                printf("Time limit:\033[35;1m %dms\033[0m\n",Config::time_limit);
                printf("Memory limit:\033[35;1m %dMB\033[0m\n",Config::mem_limit);
                printf("Threads:\033[35;1m %d\033[0m\n",Config::threads);
            }
            else if (cmd[0] == "quit" || cmd[0] == "exit")
                return 0;
            else if (cmd[0] == "run"){
                bool t = find(cmd.begin(),cmd.end(),"-t") != cmd.end();
                if (exe({"/bin/make","-j","run"}))throw "XC";
                if (t){
                    printf(RED"in.txt:"ZERO"\n");
                    if (exe({"/bin/cat","in.txt"}))throw "XC";
                    printf(RED"\nwa.txt:"ZERO"\n");
                    int ret;
                    ret = exe({"./wa"},"in.txt","wa.txt","/dev/stderr",Config::time_limit,Config::mem_limit);
                    if (ret){
                        if (ret == TLE)printf(RED"Time limit eXCeeded\n"ZERO);
                        else if (ret == MLE)printf(RED"Memory limit eXCeeded\n"ZERO);
                        else printf(RED"Runtime error\n"ZERO);
                        throw "XC";
                    }
                    if (exe({"/bin/cat","wa.txt"}))throw "XC";
                    printf(RED"\nstd.txt:"ZERO"\n");
                    ret = exe({"./std"},"in.txt","std.txt","/dev/stderr",Config::time_limit,Config::mem_limit);
                    if (ret){
                        if (ret == TLE)printf(RED"Time limit eXCeeded\n"ZERO);
                        else if (ret == MLE)printf(RED"Memory limit eXCeeded\n"ZERO);
                        else printf(RED"Runtime error\n"ZERO);
                        throw "XC";
                    }
                    if (exe({"/bin/cat","std.txt"}))throw "XC";
                    printf("\n");
                    while (1){
                        printf("How to compare: diff(d) or vimdiff(v) or cancel(c)? ");
                        char op[4096];
                        fgets(op,4096,stdin);
                        if (op[0] == 'd'){
                            printf(RED"diff:\n\n"ZERO);
                            if (exe({"/bin/diff","-Z","wa.txt","std.txt"}))throw "XC";
                            break;
                        }else if (op[0] == 'v'){
                            if (exe({"/bin/vimdiff","wa.txt","std.txt"}))throw "XC";
                            break;
                        }else if (op[0] == 'c')break;
                        else printf("Please enter d or v or c!\n");
                    }
                }else RUN::run(DUIPAI);

            }else if (cmd[0] == "spj"){
                bool t = find(cmd.begin(),cmd.end(),"-t") != cmd.end();
                if (exe({"/bin/make","-j","run"}))throw "XC";
                if (t){
                    printf(RED"in.txt:"ZERO"\n");
                    if (exe({"/bin/cat","in.txt"}))throw "XC";
                    printf(RED"\nwa.txt:"ZERO"\n");
                    int ret;
                    ret = exe({"./wa"},"in.txt","wa.txt","/dev/stderr",Config::time_limit,Config::mem_limit);
                    if (ret){
                        if (ret == TLE)printf(RED"Time limit eXCeeded\n"ZERO);
                        else if (ret == MLE)printf(RED"Memory limit eXCeeded\n"ZERO);
                        else printf(RED"Runtime error\n"ZERO);
                        throw "XC";
                    }
                    if (exe({"/bin/cat","wa.txt"}))throw "XC";
                    printf(RED"\nSPJ output:"ZERO"\n");
                    ret = exe({"./std","in.txt","wa.txt"});
                    printf("\n");
                    printf("SPJ Returned "LBLUE"%d\n"ZERO,ret);
                }else RUN::run(SPJ);

            }else if (cmd[0] == "tle"){
                bool t = find(cmd.begin(),cmd.end(),"-t") != cmd.end();
                if (exe({"/bin/make","-j","rand","wa"}))throw "XC";
                if (t){
                    printf(RED"in.txt:"ZERO"\n\n");
                    if (exe({"/bin/cat","in.txt"}))throw "XC";
                    printf("\n\n");
                }else RUN::run(DETECT);
            }
            else if (cmd[0] == "gen"){
                int n = Parse::range(cmd,1,1,INT_MAX);
                bool t = find(cmd.begin(),cmd.end(),"-t") != cmd.end();
                if (exe({"/bin/make","-j","rand","std"}))throw "XC";
                if (t){
                    printf(RED"./rand output:\n\n"ZERO);
                    if (exe({"./rand",to_string(n)},"/dev/stdin","in.txt"))throw "XC";
                    if (exe({"/bin/cat","in.txt"}))throw "XC";
                    printf("\n");
                    printf(RED"./std output:\n"ZERO);
                    if (exe({"./std"},"in.txt"))throw "XC";
                    printf("\n");
                }else RUN::gen_main(n);
            }else if (cmd[0] == "exe"){
                Parse::least(cmd,1);
                if (cmd[1] == "r" || cmd[1] == "rand"){
                    cmd.erase(cmd.begin(),cmd.begin() + 1);
                    cmd.insert(cmd.begin(),"./rand");
                    exe(cmd);
                }else if (cmd[1] == "w" || cmd[1] == "wa"){
                    cmd.erase(cmd.begin(),cmd.begin() + 1);
                    cmd.insert(cmd.begin(),"./wa");
                    exe(cmd);
                }else if (cmd[1] == "s" || cmd[1] == "std"){
                    cmd.erase(cmd.begin(),cmd.begin() + 1);
                    cmd.insert(cmd.begin(),"./std");
                    exe(cmd);
                }else printf(RED"ERROR:"ZERO" unknown command "YELLOW"%s"ZERO"!\n",cmd[1].c_str());
            }
            else if (cmd[0] == "XC" || cmd[0] == "xc"){
                printf(RED"LONG LIVE XC!!!\n\n"ZERO);
                printf(YELLOW
                    "*     *     *******\n"
                    " *   *     *       \n"
                    "  * *      *       \n"
                    "   *       *       \n"
                    "  * *      *       \n"
                    " *   *     *       \n"
                    "*     *     *******\n"
                ZERO);
            }
            else
                printf("\033[31;1mError:\033[0m \033[33;1m%s\033[0m is not a valid command!\n",cmd[0].c_str());
        }catch(...){}
        killed = false;
        signal(SIGINT,SIG_DFL);
        free(s);
    }
    return 0;
}