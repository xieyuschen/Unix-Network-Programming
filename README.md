# Unix-Network-Programming
本项目在`Ubuntu 20.04`系统,gcc版本`9.3.0`.基于`UNIX网络编程卷1：套接字联网API`进行学习。

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