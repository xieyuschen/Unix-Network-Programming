# Part7
非阻塞与阻塞相对，首先要理解阻塞的含义：例如目前需要从`sockfd`中读取
一定的数据，但目前`sockfd`的缓冲区为空，那么程序就会陷入阻塞。那么我们
就要关注非阻塞的实现:  
## 先验知识
### 阻塞IO
当用户进程调用了recvfrom这个系统调用，kernel就开始了IO的第一个阶段：准备数据。对于network io来说，很多时候数据在一开始还没有到达（比如，还没有收到一个完整的UDP包），这个时候kernel就要**等待足够的数据到来**。用户这里整个进程会被阻塞，去等待**内核调用方法的返回**。当kernel一直等到数据准备好了，它就会将数据从kernel中拷贝到用户内存，然后kernel返回结果，用户进程才解除block的状态，重新运行起来。  
所以，blocking IO的特点就是在IO执行的两个阶段（等待数据和拷贝数据两个阶段）都被block了。  
### 非阻塞IO
阻塞IO时，也即系统调用没有返回所以导致用户阻塞等待IO。那么非阻塞的话，并不会阻塞，而是立刻返回。对应上面的例子，如果kernel的数据并没有准备好，
那么**并不会阻塞，而是直接返回告诉用户目前数据没有就绪**。那么**在非阻塞式IO中，用户进程其实是需要不断的主动询问kernel数据准备好了没有**。
当非阻塞api被调用的时候，立刻返回。然后当数据从IO返回时，调用者会通知数据就绪，这个步骤叫做**回调**。  

[该博客值得一读](https://medium.com/ing-blog/how-does-non-blocking-io-work-under-the-hood-6299d2953c74)
大部分非阻塞框架是一个无限循环用于轮询数据是否就绪。这可能很消耗CPU资源，所以说这部分被做了一定优化。通过`epoll`,`kqueue`,`IOCP`可以将时间优化到`O(number to monitor)`.比`poll`与`select`优化是巨大的。
那么注意到这里，select的操作应该是在开始IO的时候（即便数据没有就绪）就可以被`FD_ISSET`所发现，然后一直被发现到IO结束。
```
Another important “optimization” is that a call tokqueue(..) or epoll(..) blocks if there is no ready data in the FDs of the interest list.
```
之后可以去深入关注`epoll`的实现模式。
### CPU&IO层面的阻塞
- CPU-bound blocking  
CPU层面的阻塞主要来自于CPU仍在对数据进行计算。那么此时被阻塞是因为处理器仍在被计算。
- IO-bound blocking  
IO层面阻塞主要是在等待从输入源获取数据，在等待的时候kernel会通知进程IO处无数据以及将线程调为休眠状态。也即此时线程**没有被处理器执行**。
## 为什么使用非阻塞IO
可以使用更少的线程，开销更小。一个线程通常来说需要1MB左右的内存以及上下文切换的开销，那么采用非阻塞模型的开销会更小。
