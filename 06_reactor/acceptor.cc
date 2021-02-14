/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */
#include "acceptor.h"


Acceptor::Acceptor(int port) : port_(port) {
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    sock::make_nonblocking(listenfd_);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int rt1 = bind(listenfd_, 
                reinterpret_cast<struct sockaddr *>(&server_addr),
                sizeof(server_addr));
    if(rt1 < 0) {
        std::cerr << "bind failed" << std::endl;
    }
    int rt2 = listen(listenfd_, backlog_);
    if(rt2 < 0) {
        std::cerr << "Listen failed" << std::endl;
    }
}   
