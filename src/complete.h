#ifndef _XC_DUIPAI_COMPLETE_H_
#define _XC_DUIPAI_COMPLETE_H_

#include "common.h"
#include <readline/readline.h>

namespace Complete{
    char *commands[] = {
        "run",
        "spjcmp",
        "spjcheck",
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
        "cd",
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

#endif