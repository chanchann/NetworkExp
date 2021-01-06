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

    bool checkErr(int i);
    int getepollFd() const { return _epollFd; }
    struct epoll_event getEvent(int i);
private:
    int _epollFd;
    std::vector<struct epoll_event> _eventList;
};

#endif  // EPOLL_H