#include "unp.h"
int main(int argc,char **argv){
    int sockfd;//socket descriptor
    struct sockaddr_in servaddr;
    if(argc!=2)
        err_quit("tcpcli IpAddress");

    //创建socket
    sockfd=Socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));

    
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htonl(SERV_PORT);

    //argv[0] passes the server ip
    //这里将可读ip转换为二进制，写入servaddr中去
    inet_pton(AF_INET,argv[0],&servaddr.sin_addr);

    //sockfd表示该连接绑定socket的描述符，
    Connect(sockfd,&servaddr,sizeof(servaddr));
    str_cli(stdin,sockfd);
    exit(0);
}

//FILE*表示要绑定的输入源,sockfd表示socket描述符
void str_cli(FILE* fp,int sockfd){
    char sendline[MAXLINE],recvline[MAXLINE];
    //从客户端程序输入中读取
    while (Fget(sendline,MAXLINE,fp))
    {
        //写入socket中去，传送到服务端
        Writen(sockfd,sendline,strlen(sendline));
        if(Readline(sockfd,recvline,MAXLINE)==0){
            err_quit("remote server terminated");
        }
        Fputs(recvline,stdout);
    }
    
}