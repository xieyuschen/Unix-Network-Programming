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
    int i,maxi,maxfd,listenfd, connfd,sockfd;
    //最多有1024个客户端
    int nready,client[FD_SETSIZE];
    ssize_t n;
    pid_t childpid;
    fd_set rset,allset;
    char buf[MAXLINE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("bind successfully\n");
    //把地址信息与监听套接字绑定
    bind(listenfd, &servaddr, sizeof(servaddr));

    //开始监听
    printf("start listen\n");
    listen(listenfd, LISTENQ);
    maxfd=listenfd;
    maxi=-1;
    for(i=0;i<FD_SETSIZE;i++){
        client[i]=-1;
    }
    FD_ZERO(&allset);
    FD_SET(listenfd,&allset);
    //signal(SIGCHLD,sig_chld);
    int j=0;
    for (;;)
    {
        printf("It's %d turn\n",j++);
        rset=allset;
        nready=select(maxfd+1,&rset,NULL,NULL,NULL);

        if(FD_ISSET(listenfd,&rset)){//new client connection
            clilen=sizeof(cliaddr);
            connfd=accept(listenfd,&cliaddr,&clilen);
            printf("connect successfully,socket id is %d",connfd);
            for(i=0;i<FD_SETSIZE;i++){
                if(client[i]<0){
                    client[i]=connfd;
                    break;
                }
            }
            if(i==FD_SETSIZE){
                printf("Too many clients\n");
                exit(0);
            }
            FD_SET(connfd,&allset);
            if(connfd>maxfd){
                maxfd=connfd;
            }
            if(i>maxi){
                maxi=i;
            }
            if(--nready<=0){
                continue;
            }
        }
        
        for(i=0;i<=maxi;i++){
            if((sockfd=client[i])<0){
                continue;
            }

            if(FD_ISSET(sockfd,&rset)){//socket可读
                //如果收到客户端结束就清理
                if((n=readline(sockfd,buf,MAXLINE))<=0){
                    //printf("Close socket!,read %d\n",n);
                    close(sockfd);
                    FD_CLR(sockfd,&allset);
                    client[i]=-1;
                }else{
                    writen(sockfd,buf,strlen(buf));
                    printf("Read String: %s\n",buf);
                }

                if(--nready<=0){
                    break;
                }
            }
        }
    }
}

