## SO_LINGER

https://time.geekbang.org/column/article/125806

```cpp
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

struct linger {　
    int l_onoff;　　　　/* 0=off, nonzero=on */　
    int l_linger;　　　　/* linger time, POSIX specifies units as seconds */
}

struct linger so_linger;
so_linger.l_onoff = 1;
so_linger.l_linger = 0;
setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &so_linger,sizeof(so_linger));
```

1. 如果l_onoff为 0，那么关闭本选项。l_linger的值被忽略，这对应了默认行为，close 或 shutdown 立即返回。如果在套接字发送缓冲区中有数据残留，系统会将试着把这些数据发送出去。

2. 如果l_onoff为非 0， 且l_linger值也为 0，那么调用 close 后，会立该发送一个 RST 标志给对端，该 TCP 连接将跳过四次挥手，也就跳过了 TIME_WAIT 状态，直接关闭。这种关闭的方式称为“强行关闭”。 在这种情况下，排队数据不会被发送，被动关闭方也不知道对端已经彻底断开。只有当被动关闭方正阻塞在recv()调用上时，接受到 RST 时，会立刻得到一个“connet reset by peer”的异常。

**非常危险的行为，不值得提倡。但可以作为模拟一些异常情况**

3. 如果l_onoff为非 0， 且l_linger的值也非 0，那么调用 close 后，调用 close 的线程就将阻塞，直到数据被发送出去，或者设置的l_linger计时时间到。

