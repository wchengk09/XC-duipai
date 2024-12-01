#ifndef _XC_DUIPAI_CONFIG_H_
#define _XC_DUIPAI_CONFIG_H_

#include "common.h"

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

#endif