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

# 更进一步:遇到错误的时候的处理
在网络方面，最不缺的就是异常。随时都可能出现问题，那么作为网络编程，我们就需要对此
进行一些理解，同时理解Part2部分代码的一些不完备的地方。  
- 服务端结束  
服务端&客户端建立连接后，关闭服务端。在客户端的表现如下：
```sh
user@user:~/.Project/Unix-Network-Programming/Part2/build$ ./client 127.0.0.1
Step into str_cli
# 第一次输入与回射
h
h
#服务端关闭，第一次输入回车后无反应
end
#第二次输入，程序结束
end2
user@user:~/.Project/Unix-Network-Programming/Part2/build$ 
```
那么为什么第一次程序没有退出，而第二次程序退出？
在服务端关闭连接之后，发生了什么事情呢？  
1. 服务端关闭连接，发回一个FIN给客户端，然后客户端回复一个ACK
2. 客户端向socket中写`end`（因为只有服务端发了FIN，客户端没有）
3. 写入之后，客户端直接开始等待从socket中读服务端的返回，此处阻塞。但是服务端给了一个`RST`,但客户端
没有看到（因为他阻塞在readline).所以客户端读到了一个EOF，也就是啥也不会输出。
4. 再次写入，此时第二次失败，直接内核发出一个`SIGPIPE`信号，程序直接强制退出。
