/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */


#include "blockQue.h"

// 相当于一个Producer
void BlockQue::push(int fd) {
    // 一定要先加锁，因为有多个线程需要读写队列
    // 使用括号减小guard锁的作用范围
    {
        std::lock_guard<std::mutex> lck(_mutex);
        _fdQue.push(fd);
        std::cout << "Push : " << fd << std::endl;
    }
    _cv.notify_one();
}

// 相当于Consumer
int BlockQue::pop() {
    std::unique_lock<std::mutex> lck(_mutex);
    _cv.wait(lck, [this] { return !_fdQue.empty(); });
    int fd = _fdQue.front();
    _fdQue.pop();
    std::cout << "pop : " << fd << std::endl;
    return fd;
}




