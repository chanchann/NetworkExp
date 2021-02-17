#ifndef EPOLL_H
#define EPOLL_H
#include <sys/epoll.h>
#include <vector>
#include <iostream>
#include <unistd.h> // close
#include <cstring> // strerror

class Epoll {
public:
    explicit Epoll(int maxEvents = 1024);
    ~Epoll();
    int add(int fd, uint32_t events);
    int mod(int fd, uint32_t events);  // not use and not finish
    int del(int fd);
    int wait(int timeOutMs);

    int getepollFd() const { return epollFd_; }
    struct epoll_event getEvent(int i);
private:
    int epollFd_;
    std::vector<struct epoll_event> eventList_;
};

#endif  // EPOLL_H