#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h> // close
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <memory>
#include <iostream>
#include <vector>
#include "select.h"
#include "../../lib/common.h"
#define BUFSIZE 1024
class Server {
public:
    Server(int port = 10000, int backlog = 1024);
    ~Server();
    void run();

private:
    bool InitSocket();
    void AcceptConnection();
    void Echo(int fd);
    ssize_t Read(int fd, void *buf, size_t count);
    ssize_t Write(int fd, const void *buf, size_t count);

private:
    int _sockfd;
    int _port;
    int _backlog;
    char _buf[BUFSIZE];

    std::unique_ptr<Select> _select;
};

#endif // SERVER_H