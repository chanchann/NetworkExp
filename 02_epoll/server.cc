/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */

#include "server.h"

Server:: Server(int port, int backlog) : 
                _port(port), _backlog(backlog),
                epoll(make_unique<Epoll>()) {
    if(!InitSocket()) {
        std::cout << "[ Server::run() ] : socket init failed " << std::endl;
        return;
    }
}

Server::~Server() {
    close(_sockfd);
}

void Server::run() {
    epoll->add(_sockfd, EPOLLIN | EPOLLET);
    while(true) {
        int n = epoll->wait(-1);
        std::cout << "epoll wait wakeup" << std::endl;
        for(int i = 0; i < n; i++) {
            int event_fd = epoll->getEvent(i).data.fd;
            if(epoll->checkErr(i)) 
                continue;
            else if(_sockfd == event_fd) { // new connection
                // TODO : Some details don't understand
                AcceptConnection();
            } else {
                std::cout << "get event on socket fd == " << event_fd << std::endl;
                echo(event_fd);
            }
        } 
    }
}

bool Server::InitSocket() {
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(_sockfd == -1) {
        std::cout << "[ Server::initSocket ]  create socket failed" << std::endl;
        return false;
    }

    SetNonBlock(_sockfd);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    std::cout << "Server port is : " << _port << std::endl;
    
    int opt = 1;
    setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(_sockfd, 
            reinterpret_cast<struct sockaddr *>(&server_addr),
            sizeof(server_addr)) == -1) {
        std::cout << "[ Server::initSocket ] Bind failed" << std::endl;
        return false;
    }
    if(listen(_sockfd, _backlog) == -1) {
        std::cout << "[ Server::initSocket ] Listen failed" << std::endl;
        return false;
    }
    return true;    
}

void Server::SetNonBlock(int fd) {
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

void Server::AcceptConnection() {
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    int fd = accept(_sockfd, 
        reinterpret_cast<struct sockaddr *>(&ss),
        reinterpret_cast<socklen_t *>(&slen));
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
            } else 
                std::cout << " There is no data available right now, try again later" << std::endl;
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



