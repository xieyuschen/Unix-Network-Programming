# 关于本part
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