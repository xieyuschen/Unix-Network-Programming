# Part 8
多线程编程
## 编译
编译时，因为`pthread`不是linux内置的库，所以要加上编译选项。
- 直接使用GCC  
```
gcc -I../../unpv13e/lib client.c -w -o  client -pthread
```
但在服务端的时候，由于使用`tcp_listen`,所以没有链接上。不知道什么情况，还有待探究。
- 使用CMake 
使用CMake暂时没有解决这个问题。
