#include "unp.h"
#include <pthread.h>
#include "util.h"

void *copyto(void*);
static int sockfd;
static FILE *fp;
void str_cli(FILE *fp_arg,int sockfd_arg){
    char recvline[MAXLINE];
    pthread_t tid;
    sockfd=sockfd_arg;
    fp=fp_arg;

    pthread_create(&tid,NULL,copyto,NULL);
    while(readline(sockfd,recvline,MAXLINE)>0){
        fputs(recvline,stdout);
    }
}
void* copyto(void* arg){
    char sendline[MAXLINE];
    while(fgets(sendline,MAXLINE,fp)!=NULL){
        writen(sockfd,sendline,strlen(sendline));
    }
    shutdown(sockfd,SHUT_WR);
    return NULL;
}
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
