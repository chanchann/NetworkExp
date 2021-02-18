/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */

#include "server.h"

Server:: Server(int port) : 
                port_(port), 
                epoll(make_unique<Epoll>()) {
    if(!InitSocket()) {
        std::cout << "[ Server::run() ] : socket init failed " << std::endl;
        return;
    }
}

Server::~Server() {
    close(listenfd_);
}

void Server::run() {
    epoll->add(listenfd_, EPOLLIN | EPOLLET);
    while(true) {
        int n = epoll->wait(-1);
        std::cout << "epoll wait wakeup and " << n << " events coming..." << std::endl;
        for(int i = 0; i < n; i++) {
            int event_fd = epoll->getEvent(i).data.fd;
            if(epoll->checkErr(i)) 
                continue;
            else if(listenfd_ == event_fd) { // new connection
                AcceptConnection();
            } else {
                std::cout << "get event on socket fd == " << event_fd << std::endl;
                echo(event_fd);
            }
        } 
    }
}

bool Server::InitSocket() {
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd_ == -1) {
        std::cout << "[ Server::initSocket ]  create socket failed" << std::endl;
        return false;
    }

    SetNonBlock(listenfd_);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    std::cout << "Server port is : " << port_ << std::endl;
    
    int opt = 1;
    setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(listenfd_, 
            reinterpret_cast<struct sockaddr *>(&server_addr),
            static_cast<socklen_t>(sizeof(server_addr))) == -1) {
        std::cout << "[ Server::initSocket ] Bind failed" << std::endl;
        return false;
    }
    if(listen(listenfd_, 1024) == -1) {
        std::cout << "[ Server::initSocket ] Listen failed" << std::endl;
        return false;
    }
    // https://blog.csdn.net/u013286409/article/details/45366075
    signal(SIGPIPE, SIG_IGN);
    return true;    
}

void Server::SetNonBlock(int fd) {
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

void Server::AcceptConnection() {
    struct sockaddr_in client_addr;
    socklen_t clitlen = sizeof(client_addr);
    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    int fd = accept(listenfd_, 
        reinterpret_cast<struct sockaddr *>(&client_addr),
        &clitlen);
    if(fd < 0) {
        std::cout << "[ Server::AcceptConnection ] Accept failed" << std::endl;
        return;
    }
    SetNonBlock(fd);
    epoll->add(fd, EPOLLIN | EPOLLET);
}

void Server::echo(int fd) {
    char buf[512];  
    int n = 0;
    while(true) {
        if((n = read(fd, buf, sizeof(buf))) < 0) {
            if(errno != EAGAIN) {
                std::cout << "[ Server::echo ] read error" << std::endl;
                epoll->del(fd); // 摘除节点
                close(fd);
            } 
            break;
        } else if(n == 0) {
            // finish
            epoll->del(fd);
            close(fd);
            std::cout << "client[ " << fd << " ] closed connection\n" << std::endl;
            break;
        } else {
            for(int i = 0; i < n; ++i) {
                buf[i] = rot13_char(buf[i]);
            }
            if(write(fd, buf, n) < 0) {
                std::cout << "[ Server::echo ] write error" << std::endl;
                return;
            }
        }
    }
    
}

char Server::rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}



