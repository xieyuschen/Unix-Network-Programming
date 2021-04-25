#include "unp.h"
#include "util.h"
#define CLI_VERSION 2
int main(int argc,char **argv){
    int sockfd;//socket descriptors
    struct sockaddr_in servaddr;
    if(argc!=2){
        printf("no ip address\n");
        exit(0);

    }
        
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));

    
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERV_PORT);

    //argv[1] passes the server ip
    //这里将可读ip转换为二进制，写入servaddr中去
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    //sockfd表示该连接绑定socket的描述符，
    connect(sockfd,&servaddr,sizeof(servaddr));
    switch (CLI_VERSION)
    {
    case 1:
        str_cli(stdin,sockfd);
        break;
    case 2:
        str_cli2(stdin,sockfd);
        break;
    default:
        break;
    }
    exit(0);
}

void str_cli(FILE* fp,int sockfd){
    int maxfdp1;
    fd_set rset;
    char sendline[MAXLINE],recvline[MAXLINE];
    FD_ZERO(&rset);
    for(;;){
        FD_SET(fileno(fp),&rset);
        FD_SET(sockfd,&rset);
        maxfdp1=max(fileno(fp),sockfd)+1;
        select(maxfdp1,&rset,NULL,NULL,NULL);
        if(FD_ISSET(sockfd,&rset)){
            if(readline(sockfd,recvline,MAXLINE)==0){
                printf("exit for readline is empty");
                exit(0);
            }
            fputs(recvline,stdout);
        }
        if(FD_ISSET(fileno(fp),&rset)){
            if(fgets(sendline,MAXLINE,fp)==NULL)
                return;
            writen(sockfd,sendline,strlen(sendline));
        }
    }


}
void str_cli2(FILE* fp,int sockfd){
    int maxfdp1,stdineof=0;
    fd_set rset;
    char buf[MAXLINE];
    char sendline[MAXLINE], recvline[MAXLINE];
    int n;
    FD_ZERO(&rset);
    int i=0;
    for(;;){
        printf("It's %d turn\n",i++);
        if(stdineof==0){
            FD_SET(fileno(fp),&rset);
        }
        FD_SET(sockfd,&rset);
        maxfdp1=max(fileno(fp),sockfd)+1;
        select(maxfdp1,&rset,NULL,NULL,NULL);
        // if(FD_ISSET(sockfd,&rset)){//socket可读
        //     printf("socket is valid to read\n");
        //     //从socket读数据,如果读失败了往下走
        //     if(readline(sockfd,buf,MAXLINE)==0){
        //         if(stdineof==1){
        //             printf("exit for stdineof is 1\n");
        //             return;
        //         }else{
        //             printf("exit for stdineof is not 1\n");
        //             exit(0);
        //         }
        //     }
        //     printf("-----%s",buf);
        //     fputs(buf,stdout);
        // }

        // //stdin可读
        // if(FD_ISSET(fileno(fp),&rset)){
        //     printf("stdin is valid to read\n");
        //     if(readline(fileno(fp),buf, MAXLINE)==-1){
        //         stdineof=1;
        //         shutdown(sockfd,SHUT_WR);
        //         FD_CLR(fileno(fp),&rset);
        //         printf("Next turn\n");
        //         continue;
        //     }
        //     printf("-----%s",buf);
        //     writen(sockfd,buf,n);
        // }

        if (FD_ISSET(sockfd, &rset)) {  /* socket is readable */
            if (readline(sockfd, recvline, MAXLINE) == 0)
                exit(0);
            fputs(recvline, stdout);
        }
        if (FD_ISSET(fileno(fp), &rset)) {  /* input is readable */
            if (fgets(sendline, MAXLINE, fp) == NULL)
                return;     /* all done */
            writen(sockfd, sendline, strlen(sendline));
        }
    }
}