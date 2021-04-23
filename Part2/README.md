# Part2的基本知识
&emsp;&emsp;Part2对`SIGCHLD`信号进行了捕获。那么首先需要了解:  
- 信号是什么，这里捕获的信号又是做什么的。
- 信号用于通知父进程清理僵死子进程，那么进程生命周期如何管理

## 信号 
&emsp;&emsp;信号是一种沟通机制。通常被进程间通信&进程与内核间通信使用，用于在某事发生的时候通知另一个进程&kernal。
- SIGCHLD信 
该信号用于通知父进程，通知其某一个子进程结束。

## 进程周期管理
&emsp;&emsp;一个进程通过`exit`系统调用正常退出，或者接收到`SIGKILL`等信息被强行停止，此时用户态下的资源全部被清理掉，但是内核
中的PCB还存在，该进程的状态会被置为`ZOMBIE`。那么什么时候该`ZOMBIE`会被清理呢？  
- 僵死子进程的父进程结束，此时子进程变成孤儿子进程，将被移交给`init`进程并释放PCB内核中的信息。
- 在子进程结束时，会对父进程发送一个`SIGCHLD`信号，父进程捕获这个信号并清理子进程，这样就可以快速释放内核空间。

## defunct状态进程
defunct状态和zombie状态的进程类似。defunct状态进程表示进程已经结束，但还在进程表中。
在将信号处理代码注释掉后，启动服务端与客户端。在Part2中我们建立了5个socket连接，启动之后停止client。
```c
    printf("start listen\n");
    listen(listenfd, LISTENQ);
    //signal(SIGCHLD,sig_chld);
    for (;;){//blabla}
```
此时运行
```sh
#使用ps命令是看不到defunct命令的
ps -a
```
我们看到：
```
 421626 pts/10   00:00:00 server
 421628 pts/10   00:00:00 server <defunct>
 421629 pts/10   00:00:00 server <defunct>
 421630 pts/10   00:00:00 server <defunct>
 421631 pts/10   00:00:00 server <defunct>
 421632 pts/10   00:00:00 server <defunct>
 ```

 # 改进
 我们需要对对应的就是对`SIGCHLD`进行处理，就可以解决这个问题。
 ```c
 //在listen后加入改代码，就可以完成
signal(SIGCHLD,sig_chld);

//sig_chld的实现
void sig_chld(int signo){
    pid_t pid;
    int stat;
    while((pid=waitpid(-1,&stat,WNOHANG))>0){
        printf("child %d terminated",pid);
    }
    return;
}
```
