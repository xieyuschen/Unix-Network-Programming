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