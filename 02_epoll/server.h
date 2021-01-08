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
#include <sys/types.h>  
#include <memory>
#include <iostream>
#include "../lib/common.h"
#include "epoll.h"

class Server {
public:
    Server(int port = 10000, int backlog = 1024);
    ~Server();
    void run();
private:
    bool InitSocket();
    void SetNonBlock(int fd);
    void AcceptConnection();
    void echo(int fd);   // TODO : 这里可以设置一个回调函数
    char rot13_char(char c);

private:
    int _port = -1;
    int _sockfd;
    int _backlog = 1024;
    // epoll
    std::unique_ptr<Epoll> epoll;
};


#endif // SERVER_H