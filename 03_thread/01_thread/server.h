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

#define MAX_LINE 16384

class Server {
public:
    Server(int port = 10000);
    ~Server();
    void run();

private:
    bool InitSocket();
    int AcceptConnection();
    static void LoopEcho(int fd);
private:
    int _listenfd;
    int _port;
    int _backlog = 1024;
};


#endif // SERVER_H