#include "unp.h"
#include<syslog.h>
#define MAXFD 64
extern int daemon_proc;
int daemon_init(const char* pname,int facility){
    int i;
    pid_t pid;
    if((pid=fork())<0){
        return -1;
    }else if(pid){
        exit(0);
    }

    if(setsid()<0){
        return -1;
    }
    signal(SIGHUP,SIG_IGN);
    if((pid=fork())<0){
        return -1;
    }else if(pid){
        exit(0);
    }

    daemon_proc=1;
    chdir("/");
    for(i=0;i<MAXFD;i++){
        close(i);
    }

    const char* DevNull="/dev/null";
    open(DevNull,O_RDONLY);
    open(DevNull,O_RDWR);
    open(DevNull,O_RDONLY);
   
    openlog(pname,LOG_PID,facility);
    return 0;
}
