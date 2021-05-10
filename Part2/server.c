#include "unp.h"
#include "util.h"
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
void sig_chld(int signo){
    pid_t pid;
    int stat;
    while((pid=waitpid(-1,&stat,WNOHANG))>0){
        printf("child %d terminated",pid);
    }
    return;
}
int main(int argc, char **argv)
{
    //服务端需要始终有一个监听套接字，一/若干个连接套接字
    int listenfd, connfd;
    //建立连接的socket被放在子线程上进行
    pid_t childpid;
    //暂时不知道干啥的
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    //地址被指定为INADDR_ANY，具体做什么的
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("bind successfully\n");
    //把地址信息与监听套接字绑定
    bind(listenfd, &servaddr, sizeof(servaddr));

    //开始监听
    printf("start listen\n");
    listen(listenfd, LISTENQ);
    signal(SIGCHLD,sig_chld);
    for (;;)
    {
        clilen = sizeof(cliaddr);
        printf("wait to accpet\n");
        if((connfd = accept(listenfd, &cliaddr, &clilen))<0){
            if(errno==EINTR)continue;
        };
        //为0表示是子进程
        /*
            强制关闭客户端
            wait to accpet
            receive a connection,sockfd is -1
        */
        printf("receive a connection,sockfd is %d\n", connfd);
        if ((childpid = fork()) == 0)
        {
            //子进程中关闭连接套接字，
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
    }
}

