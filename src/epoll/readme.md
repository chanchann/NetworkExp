## Experiments

```
$./epoll
epoll_wait wakeup
epoll_wait wakeup
epoll_wait wakeup
get event on socket fd == 6
epoll_wait wakeup
get event on socket fd == 5
epoll_wait wakeup
get event on socket fd == 5
epoll_wait wakeup
get event on socket fd == 6
epoll_wait wakeup
get event on socket fd == 6
epoll_wait wakeup
get event on socket fd == 6
epoll_wait wakeup
get event on socket fd == 5
```

```

$telnet 127.0.0.1 43211
Trying 127.0.0.1...
Connected to 127.0.0.1.
Escape character is '^]'.
fasfsafas
snfsfnsnf
^]
telnet> quit
Connection closed.
```



## explicit

If I use explicit constructor, do I need to put the keyword in both .h and .cpp files?

No, it is not. The explicit keyword is only permitted in the header;

https://stackoverflow.com/questions/273630/if-i-use-explicit-constructor-do-i-need-to-put-the-keyword-in-both-h-and-cpp

## Setup std::vector in class constructor

Just do:

MyClass::MyClass(int m_size) : size(m_size), vec(m_size, 0)

https://stackoverflow.com/questions/11415469/setup-stdvector-in-class-constructor

## Error: default argument given for parameter after previous specification

You can declare default arguments in the class declaration or in the function definition, but not both.

https://stackoverflow.com/questions/13295530/error-default-argument-given-for-parameter-after-previous-specification/13295570

https://blog.csdn.net/jkhere/article/details/11852971

## C++ const in getter

const bool isReady()  NO!!!

bool isReady() const  Yes!!!

It makes no sense to return a const bool because it is a copy anyway. 

The second const is needed for const correctness, which is not used for speed reasons but to make your program more reliable and safe.

## C++: Vector bounds

https://stackoverflow.com/questions/14015632/c-vector-bounds

## std::error_runtime

Not fully understand how to handle exception yet.

https://stackoverflow.com/questions/1569726/difference-stdruntime-error-vs-stdexception

## What does EAGAIN mean?

EAGAIN is often raised when performing non-blocking I/O. It means "there is no data available right now, try again later".

https://stackoverflow.com/questions/4058368/what-does-eagain-mean