#include "unp.h"
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
        printf("Get string from client stdin,%s,length is %d ",sendline,strlen(sendline));
        //写入socket中去，传送到服务端
        write(sockfd,sendline,strlen(sendline));
        ssize_t n;
        int len = 0;
        
        ioctl(sockfd, FIONREAD, &len);
        if (len > 0) {
            len = read(sockfd, recvline, len);
            printf("%d",len);
        }
        fputs(recvline,stdout);
        // while((read(sockfd,recvline,10)!=0)){
        //     fputs(recvline,stdout);
        // }
        // if(read(sockfd,recvline,MAXLINE)==0){
        //     printf("Failed to read from server side\n");
        //     exit(0);
        // }
    }
}