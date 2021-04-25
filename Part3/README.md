# Part3
I/O多路复用。也即我们不希望被阻塞在某处，比如说在Part2中:
```c
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
    }
}
```
在`writen`之后直接阻塞在readline，此时服务端给写回的信息并没有被收到。所以
我们引入`select`,不阻塞在一个io上面，而是同时对两个输入端进行监听。所以在服务端关闭后，socket
会告诉服务端情况，此时就可以直接对其做处理。而不是在第一次收到RST，第二次收到SIGPIPE信号强制关
闭进程。