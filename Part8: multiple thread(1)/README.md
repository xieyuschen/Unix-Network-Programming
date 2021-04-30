# Part 8
多线程编程
## 编译
编译时，因为`pthread`不是linux内置的库，所以要加上编译选项。
### 直接使用GCC  
```
gcc -I../../unpv13e/lib client.c -w -o  client -pthread
```
但在服务端的时候，由于使用`tcp_listen`,所以没有链接上。不知道什么情况，还有待探究。  
————破案了，在包含头文件的时候还是需要把`.a`结尾的静态库给链接上。
在使用的时候
### 使用CMake 
- `pthread`找不到
CMake编译项目时解决了这个问题。首先是`pthread`的相关问题，在使用CMake时候需要加上:
需要找到`Threads`的包，然后使用`target_link_libraries`将`pthread`加上。
```
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(server PUBLIC 
    Threads::Threads
    #sth else
)
```
- `tcp_listen`找不到  
在`unp.h`的文件之中明明拥有`tcp_listen`函数的声明，但却遇到undefined reference的问题,
那么可能是静态包没有被包括。所以在CMake中添加:  
```
set(LIB_PATH /home/user/.Project/unpv13e/)
# blabla
target_link_libraries(server PUBLIC 
    Threads::Threads
    ${LIB_PATH}/libunp.a
)
```

## 关于线程的一些相关知识
### 线程与初始线程    
一个程序由`exec`启动执行时称为初始线程或主线程。这是单个线程，其余线程需要由`pthread_create`
来创建。每个线程都有一个线程ID，数据类型`pthread_t`。线程也有很多属性，可以通过`pthread_attr_t`
变量指定这些属性。空指针就是默认设置。  
### 常见函数
- `pthread_join`函数等待一个线程结束
- `pthread_srlf`函数获取自己的线程号
- `pthread_detach`函数  
一个线程是`joinable`（默认值）或者`detached`（脱离）。joinable状态的线程ID和退出状态可以留存到
另一线程对其调用`pthread_join`函数。而`detached`状态就不行，会直接释放掉资源。  

## 启动项目
本次参数传递需要额外传递:
```sh
./server 127.0.0.1 9877
./client 127.0.0.1
```
在启动server的时候，域名和端口是需要做好的。