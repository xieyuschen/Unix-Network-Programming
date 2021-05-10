#include "unp.h"
#include "util.h"
int main(int argc,char **argv){
    int sockfd;//socket descriptor
    struct sockaddr_in servaddr;
    if(argc!=2){
        printf("no ip address\n");
        exit(0);

    }

    //创建socket
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));

    
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERV_PORT);

    //argv[1] passes the server ip
    //这里将可读ip转换为二进制，写入servaddr中去
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    //sockfd表示该连接绑定socket的描述符，
    connect(sockfd,&servaddr,sizeof(servaddr));
    printf("make a connection,sockfd is %d\n",sockfd);
    str_cli(stdin,sockfd);
    exit(0);
}

//FILE*表示要绑定的输入源,sockfd表示socket描述符
void str_cli(FILE* fp,int sockfd){
    printf("Step into str_cli\n");
    char sendline[MAXLINE];
    //从客户端程序输入中读取
    while (fgets(sendline,MAXLINE,fp)!=NULL)
    {
        char recvline[MAXLINE];
        //写入socket中去，传送到服务端
        writen(sockfd,sendline,strlen(sendline));
        //从服务端获取
        readline(sockfd, recvline,MAXLINE);
        fputs(recvline,stdout);
        // while((readline(sockfd,recvline,10)!=0)){
        //     fputs(recvline,stdout);
        // }
        // if(readline(sockfd,recvline,MAXLINE)==0){
        //     printf("Failed to readline from server side\n");
        //     exit(0);
        // }
    }
}