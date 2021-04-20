#include "unp.h"

int main(int argc,char** argv){
    //服务端需要始终有一个监听套接字，一/若干个连接套接字
    int listenfd,connfd;
    //建立连接的socket被放在子线程上进行
    pid_t childpid;
    //暂时不知道干啥的
    socklen_t clilen; 
    struct sockaddr_in cliaddr,servaddr;
    listenfd=Socket(AF_INET,SOCK_STREAM,0);

    bzero(&sockaddr,sizeof(sockaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERV_PORT);
    //地址被指定为INADDR_ANY，具体做什么的
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

    //把地址信息与监听套接字绑定
    Bind(listenfd,&sockaddr,sizeof(sockaddr));
    
    //开始监听
    Listen(listenfd,LISTENQ);
    for(;;){
        clilen=sizeof(cliaddr);
        connfd=Accept(listenfd,&cliaddr,&clilen);
        //为0表示是子进程
        if((childpid=fork())==0){
            //子进程中关闭连接套接字，
            Close(listenfd);
            str_echo(connfd);
        }
        Close(connfd);
    }
}