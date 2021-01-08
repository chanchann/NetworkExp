/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */
#ifndef SERVER_H
#define SERVER_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h> // close
#include <csignal>
#include <sys/types.h>  
#include <memory>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include "blockQue.h"
#include "../lib/common.h"

#define MAX_LINE 16384

class Server {
public:
    Server(int port = 10000, int thrdNum = 3);
    ~Server();
    void run();

private:
    bool InitSocket();
    void AcceptConnection();
    static void LoopEcho(int fd);
    void thrdRun();
private:
    int _listenfd;
    int _port;
    int _backlog = 1024;
    std::vector<std::thread> _threadList;
    std::unique_ptr<BlockQue> _blockQue;
};

#endif // SERVER_H