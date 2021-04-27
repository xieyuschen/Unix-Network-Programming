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
        }else if(rc==0){//读到0字符，结束输入
            *ptr=0;
            return (n-1);
        }else{//socket不可读
            return -1;
        }
    }
    *ptr=0;
    return n;
}