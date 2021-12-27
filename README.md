# Unix-Network-Programming
本项目在 `Ubuntu 20.04`系统,gcc版本`9.3.0`.基于`UNIX网络编程卷1：套接字联网API`进行学习。
## 先前准备
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
- Q1  
`Q`:为什么要IO多路复用？  
`A`:不希望被阻塞在某一个IO上，而是希望在单个线程中可以拥有对多个IO的处理能力  
`Q`:那如何实现这个功能？
`A`:通过`select`阻塞，而不是在IO处阻塞。这样可以避免被阻塞等待一处IO可用时错过另外就绪的IO.**当然
要强调如果操作系统没有提供这个系统调用,那就不可以使用这个功能**.  

- Q2  
`Q`:什么是非阻塞IO？  
`A`:非阻塞IO不阻塞，立刻返回。无论IO是否成功，将状态码返回给用户   
`Q`:那么如果成功，数据从哪里返回？  
`A`:在非阻塞IO使用的过程中，会使用系统调用[aio_read](https://man7.org/linux/man-pages/man3/aio_read.3.html)告诉os进行了aio(Asynchronous IO)。那么立刻返回的信息标识是否成功。然后在成功之后会直接传递到结构体中去.The `asynchronous` means that this call returns as soon as the request has been enqueued  
- Q3  
`Q`:[Part8(1)](./Part8(1))中的非阻塞IO是如何实现的？  
`A`:在其中，采取自己维护一段缓冲区。然后在这段缓冲区中进行读数据。然后仍然使用了阻塞读，也即`read`函数，保证了如果可读那么在返回时数据其实数据已经被读了出来。

- Q4  
`Q`:select怎么样？  
`A`:效率低开销大&可拓展性低&需要关注内核拷贝数据   
    - 开销大:select是一个系统调用，`FD_SET`会被注册然后**传入内核**中再被传出，描述符数目很大时内核开销很大。  
    - 可拓展性低:select会在被关注的描述符可用时被激活，但它不知道**哪个描述符可用**，所以要进行一轮扫描.然后程序员要根据
    每个有效的描述符指定相应的逻辑.糟糕极了.如果使用一个for循环对有效和无效的进行检查,那就更加的糟糕了.  
    - 可监听的描述符有限,通常不超过1024个.
    - 在上一轮监听时候,可用的监听描述符会被置位,这个时候内核的数据是需要被重新拷贝的.每次都要重新拷贝效率低. 
    - 每次可用后，需要去遍历bitmap查找可操作类型，可读&可写&错误之中的一种 
`Q`:select这么拉跨,有没有可能优化?  
`A`:在select阻塞取消时候,他并不知道哪个描述符可用.但其实select停止阻塞时候在内核是知道哪个描述符的,之所以不知道是因为没有
对此部分信息进行保存.如果保存这部分信息那么就可以提高效率.  
`Q`:那select这么多缺点,他有啥优点?  
`A`:遵循POSIX标准->跨平台&对时间的控制可以被控制在微秒(ms)级别.  
`Q`:那有没有更好的函数？  
`A`:有，用`poll`或者`epoll`.  

- Q5  
`Q`:poll函数怎么用？  
`A`:注册关注描述符，指定bitmap究竟是可读&可写&错误之中的哪一种  
`Q`:那poll函数有什么优势？  
`A`:监听数目更多&不对原有数据进行修改
    - poll函数可以支持更多的描述符监听,使用链表对描述符进行监听    
    - select对注册fd_set的bitmap进行了修改。而poll没有故可以一直用，不需要重新注册  
`Q`:那poll效率与select比有没有优势？  
`A`:没有，两者一样烂。都是和定义的描述符呈线性关系。
`Q`:那咋整？  
`A`: `epoll`和`kqueue`可以带你起飞~  

- Q6 
`Q`:epoll为什么效率高？  
`A`:解决了上面`select`与`poll`的几个问题  
    - `epoll_ctl`函数在注册文件描述符的时候，会把所有fd拷贝到内核，保证只拷贝一次。
    - `epoll_ctl`为每个fd指定一个回调函数，当设备就绪唤醒等待者后，就会调用回调函数把就绪描述符加入到就绪队列中去。  
    - 可监听的描述符取决于机器的情况，可以通过`cat /proc/sys/fs/file-max`命令查看。
