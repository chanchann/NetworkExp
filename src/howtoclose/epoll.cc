#include "epoll.h"

Epoll::Epoll(int maxEvents) : 
                epollFd_(epoll_create1(0)),
                eventList_(maxEvents) 
        {}

Epoll::~Epoll() {
    close(epollFd_);
}


int Epoll::add(int fd, uint32_t events)  {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    int ret = epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event);
    if(ret == -1) 
        std::cout << "[ Epoll::add ] Add connection fd failed " << std::endl;
    return ret;
}

// not finish
int Epoll::mod(int fd, uint32_t events) {
    struct epoll_event event;
    event.events = events;
    return epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event);
}


int Epoll::del(int fd) {
    int ret = epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr);
    if(ret == -1) 
        std::cout << "[ Epoll::del ] Del connection fd failed " << std::endl;
    return ret;
}

int Epoll::wait(int timeOutMs)  {
    // epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    int eventsNum = epoll_wait(epollFd_, eventList_.data(), 
                static_cast<int>(eventList_.size()), timeOutMs);
    if(eventsNum < 0) {
        std::cout << "[ Epoll::wait(): events error ]" << std::endl;
    }
    return eventsNum;
}

struct epoll_event Epoll::getEvent(int i) {
    // This can give you runtime exception
    return eventList_.at(i);
}

bool Epoll::checkErr(int i) {
    if(eventList_[i].events & EPOLLERR ||
        eventList_[i].events & EPOLLHUP ||
        (!(eventList_[i].events & EPOLLIN))) {
        std::cout << std::string(strerror(errno)) << "epoll error\n" << std::endl;
        close(eventList_[i].data.fd);
        return true;
    }
    return false;
}






