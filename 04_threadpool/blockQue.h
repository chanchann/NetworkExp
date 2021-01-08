/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */
#ifndef BLOCKQUE_H
#define BLOCKQUE_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>


class BlockQue {
public:
    BlockQue() = default;
    ~BlockQue() = default;
    void push(int fd);
    int pop();
private:
    std::queue<int> _fdQue; 
    std::mutex _mutex;
    std::condition_variable _cv;
};

#endif // BLOCKQUE_H