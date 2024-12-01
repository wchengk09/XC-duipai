#include "src/common.h"
#include "src/complete.h"
#include "src/parse.h"
#include "src/config.h"
#include "src/run.h"
#include <readline/readline.h>
#include <readline/history.h>

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
                    printf(RED"diff:\n\n"ZERO);
                    if (exe({"/bin/diff","-Z","wa.txt","std.txt"}))throw "XC";
                }else RUN::run(DUIPAI);

            }else if (cmd[0] == "spjcheck"){
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
                    ret = exe({"./spj","in.txt","wa.txt"});
                    printf("\n");
                    printf("SPJ Returned "LBLUE"%d\n"ZERO,ret);
                }else RUN::run(SPJCHECK);
            }else if (cmd[0] == "spjcmp"){
                bool t = find(cmd.begin(),cmd.end(),"-t") != cmd.end();
                if (exe({"/bin/make","-j","run","spj"}))throw "XC";
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
                    printf(RED"SPJ output:\n\n"ZERO);
                    if (exe({"./spj","in","wa.txt","std.txt"}))throw "XC";
                }else RUN::run(SPJCMP);
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
            else if (cmd[0] == "cd"){
                Parse::least(cmd,1);
                chdir(cmd[1].c_str());
            }
            else{
                for (int i = 1;i < (int)cmd.size();i ++)
                    cmd[0] += " " + cmd[i];
                cmd.resize(1);
                cmd.insert(cmd.begin(),"-c");
                cmd.insert(cmd.begin(),"/bin/sh");
                exe(cmd);
            }
        }catch(...){}
        killed = false;
        signal(SIGINT,SIG_DFL);
        free(s);
        clean_garbage_files();
    }
    clean_garbage_files();
    return 0;
}