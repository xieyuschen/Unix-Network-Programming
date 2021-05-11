# Unix-Network-Programming
本项目在`Ubuntu 20.04`系统,gcc版本`9.3.0`.基于`UNIX网络编程卷1：套接字联网API`进行学习。
## 本仓库涉及到的主要内容:
- [Part1-easiest-socket-server&client](./Part1)  
服务端使用多进程的socket编程。  
主要对以下概念进行了实践：
    - 使用fork函数实现通过多进程处理多个socket连接
    - 使用gcc来编译执行代码
    - 使用CMake对项目进行编译执行
    - 反思多进程的一些缺点，如在进程死亡后并不会被立刻清理仍占用进程表等  
- [Part2-foucs-on-process-detail-on-server-side](./Part2)  
在本章节中，延续了Part1的部分内容，通过信号机制对多进程进行了相关处理。  
主要对以下概念进行实践：  
    - 信号的基本知识以及进程生命周期中可能产生的信号
    - 捕获与处理信号
    - 在关闭服务端时，客户端两次写入后分别获取`RST`与`SIGPIPE`信号，最终结束进程  
- [Part3-use-IO-multiplexing-on-client](./Part3)
本章在客户端使用IO多路复用模型，为了避免IO阻塞在某一个地方。  
主要对以下概念进行实践：  
    - IO多路复用的基本概念
    - IO多路复用中常见的`select()`,`FD_SET`等函数以及数据结构
    - 了解阻塞IO模型以及其效率低的原因
- [Part4-use-IO-multiplexing-on-client&server](./Part4)
本章节支持了服务端的IO多路复用模型，通过IO多路复用来完成服务端对多个套接字的监听。  
主要对以下概念进行实践：  
    - 如何通过IO多路复用模型使服务端具有处理多个socket连接的能力  
    - 如何管理和组织多个socket连接描述符
    - 对`select()`函数的进一步理解
- [Part5-make-a-daemon](./Part5)  
本章节介绍了关于守护进程的相关知识。  
主要对以下概念进行实践：  
    - 进程组与会话的概念
    - 守护进程的特征
    - 如何一步步的生成一个守护进程
 
- [Part6-advanced-IO](./Part6)  
讲述了一些高级IO函数。

- [Part7-non-blocking-IO](./Part7)
本节讲述非阻塞IO的相关知识。  
主要对以下概念进行实践：  
    - 对IO的各种形式进行了学习
    - 学习非阻塞IO的思路（立即返回与回调）
    - ~~对于非阻塞IO中轮询（不停检查IO是否就绪的行为）消耗CPU资源情况下使用阻塞的解决方案~~
    - 非阻塞IO的具体实现方式
    - 对数据缓冲区的细节实现与&了解
- [Part8(1)-thread-server](./Part8(1))
本节在服务器端使用了多线程版本进行相关连接的处理。  
主要对以下概念进行实践：  
    - 线程相关操作函数的使用
    - 线程的相关概念
    - 线程间可共享数据的特性
- More  
[sohardforaname/TinyHttpServer](https://github.com/sohardforaname/TinyHttpServer)使用C++实现的轻量级服务器。使用线程池模型&多路复用进行服务效率的提升。

# 碎碎念&QA
- `Q`:为什么要IO多路复用？  
`A`:不希望被阻塞在某一个IO上，而是希望在单个线程中可以拥有对多个IO的处理能力  
`Q`:那如何实现这个功能？
`A`:通过`select`阻塞，而不是在IO处阻塞。这样可以避免被阻塞等待一处IO可用时错过另外就绪的IO  

- `Q`:什么是非阻塞IO？  
`A`:非阻塞IO不阻塞，立刻返回。无论IO是否成功，将状态码返回给用户   
`Q`:那么如果成功，数据从哪里返回？  
`A`:在非阻塞IO使用的过程中，会使用系统调用[aio_read](https://man7.org/linux/man-pages/man3/aio_read.3.html)告诉os进行了aio(Asynchronous IO)。那么立刻返回的信息标识是否成功。然后在成功之后会直接传递到结构体中去.The `asynchronous` means that this call returns as soon as the request has been enqueued  
- `Q`:[Part8(1)](./Part8(1))中的非阻塞IO是如何实现的？  
`A`:在其中，采取自己维护一段缓冲区。然后在这段缓冲区中进行读数据。然后仍然使用了阻塞读，也即`read`函数，保证了如果可读那么在返回时数据其实数据已经被读了出来。
　　　
# 先前准备
- 本书中出现对`unp.h`的依赖，首先需要下载该书给出的依赖库  
```sh
git clone https://github.com/unpbook/unpv13e.git
```
如果速度较慢，考虑换源:
```sh
git clone https://hub.fastgit.org/unpbook/unpv13e.git
```
- 依据项目中`Readme`给出的提示进行编译:  
由于在ubuntu系统上，故有些步骤不需要。在完成后，我们所需的依赖就在`unpv13e/lib`下了。
```
Execute the following from the src/ directory:

    ./configure    # try to figure out all implementation differences

    cd lib         # build the basic library that all programs need
    make           # use "gmake" everywhere on BSD/OS systems

    cd ../libfree  # continue building the basic library
    make

    cd ../intro    # build and test a basic client program
    make daytimetcpcli
    ./daytimetcpcli 127.0.0.1

If all that works, you're all set to start compiling individual programs.
```
- gcc编译时指定`unp.h`的位置  
使用-I参数对库的位置进行指定：
```sh
# current path is /blabla/Unix-Network-Programming/Part1
gcc -I../../unpv13e/lib client.c -o client
```
项目组织如下:  
```
├── Unix-Network-Programming
│   ├── general
│   │   └── unp.h
│   ├── Part1
│   │   ├── client.c
│   │   └── server.c
│   └── README.md
└── unpv13e
    ├── lib
    │   ├── unp.h
    │   ├── etc..
    ├── etc..
```