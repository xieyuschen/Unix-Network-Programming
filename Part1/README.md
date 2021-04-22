# 从套接字读入读出
首先目前出现的问题是，在客户端写入socket后，服务端接受再写回，客户端就接收不到了。需要到下一轮来接受。  
- 在客户端，服务端并没有写入
打下输出标志，很明显关注：`Recvline Detail:3h`，但我在客户端只输入了`h`,加上回车。那么说明我的函数出现了错误。  
仔细阅读教材，发现其实`read`和`write`函数都是每次写一个单字进去，所以照着书本敲了函数进去。
```c
#include"unp.h"
ssize_t writen(int fd,const void* vptr,size_t n){
    size_t nleft;
    ssize_t nwriten;
    const char* ptr;
    ptr=vptr;
    nleft=n;
    while(nleft>0){
        if((nwriten=write(fd,ptr,nleft))<=0){
            if(nwriten<0)nwriten=0;
            else return -1;
        }
        nleft-=nwriten;
        ptr+=nwriten;
    }
    return n;
}

ssize_t readline(int fd,void* vptr,size_t maxlen){
    ssize_t n,rc;
    char c,*ptr;
    ptr=vptr;
    for(n=1;n<maxlen;n++){
        if((rc=read(fd,&c,1)==1)){
            *ptr++=c;
            if(c=='\n')break;
        }else if(rc==0){
            *ptr=0;
            return (n-1);
        }else{
            return -1;
        }
    }
    *ptr=0;
    return n;
}
```

ok，目前问题解决。

- 需要编写一个CMakeLists.txt  
目前的`run.sh`其实升级版就是一个`CMakeLists.txt`,然后和方便的build和make。当然目前项目也不大，不写问题也不大:)

# 加入CMake支持
用CMake对项目管理可以更加的优雅。这里使用CMake来完成对项目的管理：
```sh
#限制cmake最基本的版本
cmake_minimum_required(VERSION 3.16)
#给你的项目起一个名字，当然你还可以带上version
project(Part1)
#指定编译器，这里就是指定C的标准为C11，C的标准必须有
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# 生成可执行文件，注意此时还没有加上依赖什么的
add_executable(client client.c)
add_executable(server server.c)

#这里加入上面可执行文件所依赖的库，LIB_PATH的值通过cmake时
#-DLIB_PATH传入
target_include_directories(client PRIVATE ${LIB_PATH})
target_include_directories(server PRIVATE ${LIB_PATH})
```
使用`CMake`编译项目：
```sh
#start at path Unix-Network-Programming/Part1
sudo mkdir -p build
cd build
#这里我的unp.h文件在~/.Project/unpv13e/lib/ 路径下
cmake .. -DLIB_PATH=~/.Project/unpv13e/lib/
make
```

# Part1的不足
## Part1所实现的功能
`Part1`实现了简单的socket连接。从客户端到服务端的一个回射。  
- 服务端  
服务端完成了对套接字的创建，对套接字绑定协议簇以及端口。之后开启`listen`和`accept`的生活。其中listen改变了套接字的
状态，然后accept会阻塞并等待socket连接。连接完成后，会开启新进程去完成与客户端的交互，在主进程上仍然让监听套接字进
行监听。接受客户套接字输入然后再写回。
- 客户端  
客户端创建了自己的套接字。然后绑定协议簇以及端口，开启connect请求去和服务器连接。给服务端发送信息后再接受发送回来的。
## Part1还不完善的地方
使用命令查看本次编程端口9877的连接情况：
```sh
netstat -a|grep 9877
```
```sh
Proto Recv-Q Send-Q Local Address           Foreign Address         State
tcp        0      0 0.0.0.0:9877            0.0.0.0:*               LISTEN     
#此处这个是客户端的端口，然后58640应该是操作系统给分配的端口，每次不一定一样
tcp        0      0 localhost:9877          localhost:58640         ESTABLISHED
tcp        0      0 localhost:58640         localhost:9877          ESTABLISHED
```
此时关闭客户端：
```
tcp        0      0 0.0.0.0:9877            0.0.0.0:*               LISTEN     
tcp        0      0 localhost:58640         localhost:9877          TIME_WAIT  
```
一段时间后,`TIME_WAIT`状态下的内容被操作系统自动清理掉。但是假设一次性终结了大量的连接，我们
需要对其手动进行释放，否则就会产生大量僵死进程。