/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */
#ifndef ACCEPTOR_H 
#define ACCEPTOR_H 

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

#include "socket.h"

class Acceptor {
public:
    Acceptor(int port);
    ~Acceptor();
private:
    int port_;
    int listenfd_;
    int backlog_ = 1024;
};

#endif	// ACCEPTOR_H