#include"unp.h"
#include <pthread.h>
#include"util.h"

static void* doit(void *arg){
    pthread_detach(pthread_self());
    str_echo((int)arg);
    close((int)arg);
    return NULL;
}
void str_echo(int sockfd)
{
    char recvline[MAXLINE];
    //从客户端程序输入中读取
    //写入socket中去，传送到服务端
    ssize_t n;
again:
    while ((n = readline(sockfd, recvline, MAXLINE) > 0))
    {
        writen(sockfd, recvline, strlen(recvline));
        printf("Recvline Detail:%d%s", strlen(recvline), recvline);
    }
    if (n < 0)
        goto again;
    else
        printf("Read error");
}

int main(int argc,char **argv
){
    int listenfd,connfd;
    pthread_t tid;
    socklen_t addrlen,len;
    struct sockaddr *cliaddr;
    if(argc==2)
        listenfd=tcp_listen(NULL,argv[1],&addrlen);
    else if(argc==3)
        listenfd=tcp_listen(argv[1],argv[2],&addrlen);
    else{
        printf("usage: server [host] <prot>");
        exit(0);
    }
    
    cliaddr=malloc(addrlen);
    for(;;){
        len=addrlen;
        connfd=accept(listenfd,cliaddr,&len);
        pthread_create(&tid,NULL,&doit,(void*)connfd);
    }
}
