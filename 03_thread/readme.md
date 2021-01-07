# What does signal(SIGPIPE, SIG_IGN); do?

simply ignores the signal SIGPIPE

https://stackoverflow.com/questions/30799296/what-does-signalsigpipe-sig-ign-do#:~:text=answer%20was%20accepted%E2%80%A6-,signal(SIGPIPE%2C%20SIG_IGN)%3B,'t%20caught%20or%20ignored).

# Why does SIGPIPE exist?

SIGPIPE is generated exactly when the write fails with EPIPE

https://stackoverflow.com/questions/8369506/why-does-sigpipe-exist#:~:text=SIGPIPE%20is%20generated%20exactly%20when,consume%20any%20pending%20SIGPIPE%20signal%20(

## SIGPIPE

连接建立，若某一端关闭连接，而另一端仍然向它写数据，第一次写数据后会收到RST响应，此后再写数据，内核将向进程发出SIGPIPE信号，通知进程此连接已经断开。而SIGPIPE信号的默认处理是终止程序

为避免这种情况，可以选择忽略SIGPIPE信号，不执行任何动作。

https://www.cnblogs.com/kex1n/p/7662036.html

## pthread_detach(pthread_self());

https://www.geek-share.com/detail/2695883060.html

## Static member declaration c++11

https://stackoverflow.com/questions/46884960/static-member-declaration-c11