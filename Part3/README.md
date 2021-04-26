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
## 几个常用函数
- select()  

首先明确下本节比较核心的`select`函数，该函数是一个系统调用，查阅[文档](https://man7.org/linux/man-pages/man2/select.2.html),关于`select`函数的描述是这样的：  
select() allows a program to monitor multiple file descriptors,
       waiting until one or more of the file descriptors become "ready"
       for some class of I/O operation (e.g., input possible).    A file
       descriptor is considered ready if it is possible to perform a
       corresponding I/O operation (e.g., read(2), or a sufficiently
       small write(2)) without blocking.  
       select() can monitor only file descriptors numbers that are less
       than FD_SETSIZE; poll(2) and epoll(7) do not have this
       limitation.  
那么这里告诉我们`select`监听了很多文件描述符`file descriptors`,等待其就绪。

- FD_ZERO()  
This macro clears (removes all file descriptors from) set.
It should be employed as the first step in initializing a
file descriptor set.  
也即把fd_set清零用于初始化。

- FD_SET()  
This macro adds the file descriptor fd to set.  Adding a
file descriptor that is already present in the set is a
no-op, and does not produce an error.

- FD_ISSET()  
select() modifies the contents of the sets according to
the rules described below.  After calling select(), the
FD_ISSET() macro can be used to test if a file descriptor
is still present in a set.  FD_ISSET() returns nonzero if
the file descriptor fd is present in set, and zero if it
is not.

## 具体应用
首先我们关注`FD_ISSET()`函数，这个函数告诉我们:
```
After calling select(), the FD_ISSET() macro can be used to test if a file descriptor is still present in a set.  FD_ISSET() returns nonzero if the file descriptor fd is present in set, and zero if it is not.
```
如果描述符在集合里就返回非零，如果没有就返回零。这里和之前我写代码时候理解的不太一样。那么根据代码来进行分析:  
首先关注`select()`函数，去查阅下资料:
`select() blocks the program until input or output is ready on a specified set of file descriptors, or until a timer expires, whichever comes first. `  
如果都没有就绪&没有超时，那select就会被阻塞，那对应到本程序。`select(maxfdp1,&rset,NULL,NULL,NULL);`并没有指定超时，那么也就是说
它会在这里无限阻塞直到有一个描述符可用。  
简而言之，程序把阻塞的位置向上提高了一层。在过去的时候，我们会直接在一个描述符上阻塞，比如在与控制台进行I/O时我们阻塞在stdin上。但使用
select之后，阻塞就在select这里。如果出现监听的内容可读的话，阻塞解除。然后要去判断究竟是那一个描述符变为可用状态。

```c
    for(;;){
        printf("It's %d turn\n",i++);
        if(stdineof==0){
            FD_SET(fileno(fp),&rset);
        }
        FD_SET(sockfd,&rset);
        maxfdp1=max(fileno(fp),sockfd)+1;
        select(maxfdp1,&rset,NULL,NULL,NULL);
        if(FD_ISSET(sockfd,&rset)){//socket可读
            printf("socket is valid to read\n");
            //从socket读数据,如果读失败了往下走
            if(readline(sockfd,buf,MAXLINE)==0){
                if(stdineof==1){
                    printf("exit for stdineof is 1\n");
                    return;
                }else{
                    printf("exit for stdineof is not 1\n");
                    exit(0);
                }
            }
            printf("-----%s",buf);
            fputs(buf,stdout);
        }

        //stdin可读
        if(FD_ISSET(fileno(fp),&rset)){
            printf("stdin is valid to read\n");
            if(fgets(buf,MAXLINE,fp)==NULL){
                stdineof=1;
                shutdown(sockfd,SHUT_WR);
                FD_CLR(fileno(fp),&rset);
                printf("Next turn\n");
                continue;
            }
            writen(sockfd,buf,n);
        }
    }
```
