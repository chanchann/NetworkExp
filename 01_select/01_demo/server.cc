/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */

#include "server.h"

Server::Server(int port, int backlog) :
                _port(port), _backlog(backlog),
                _select(make_unique<Select>()) {
    if(!InitSocket()) {
        std::cout << "[Server::Server] Server init error" << std::endl;
        return;
    }
}

Server::~Server() {
    close(_sockfd);
}

void Server::run() {
    while(true) {
        _select->update();
        int nready = _select->select();
        if(_select->isFocus(_sockfd)) {  // new connection 
            std::cout << "New Connetion ..." <<std::endl;
            AcceptConnection();
            if(--nready == 0)  // only lfd 
                continue;
        }
        // 检测哪个client有数据就绪
        size_t client_size = _select->getClientSize();
        for(size_t i = 0; i < client_size; i++) {
            int cfd = _select->getClient(i);
            if(_select->isFocus(cfd)) {
                Echo(cfd);
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

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    std::cout << "Server port is " << _port << std::endl;

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
    _select->set(_sockfd);
    return true;  
}

void Server::AcceptConnection() {
    struct sockaddr_in clit_addr;
    socklen_t clit_addr_len = sizeof(clit_addr);
    char inet_str[INET_ADDRSTRLEN];
    int cfd = accept(_sockfd, 
                reinterpret_cast<struct sockaddr *>(&clit_addr),
                &clit_addr_len);
    if(cfd < 0) {
        std::cout << "[ Server::AcceptConnection ] Accept failed" << std::endl;
        return;
    }
    std::cout << "Recieve from [ " 
        <<  inet_ntop(AF_INET, &clit_addr.sin_addr, inet_str, sizeof(inet_str)) 
        << " : " << ntohs(clit_addr.sin_port) << " ]" << std::endl;
    _select->add(cfd);
}

void Server::Echo(int fd) {
    // 当client 关闭连接时，服务端也关闭相应连接
    ssize_t n;
    if((n = Read(fd, _buf, sizeof(_buf))) == 0) {
        _select->clear(fd);
    } else if(n > 0) {
        for(ssize_t i = 0; i < n; i++) {
            _buf[i] = toupper(_buf[i]);
            Write(fd, _buf, n);
            Write(STDOUT_FILENO, _buf, n);
        }
    }
}

ssize_t Server::Read(int fd, void* buf, size_t count) 
{
    ssize_t n;   //读到的字节数
    while(true) {
        if ( (n = read(fd, buf, count)) == -1) {
            if (errno == EINTR)
                continue;    //如果是由于信号被中断 则再试一次
            else {
                std::cout << "[Server::Read] read error " << std::endl;
                return -1;
            }
        }
        break;
    }
    return n;
}

ssize_t Server::Write(int fd, const void *buf, size_t count) {
    ssize_t n;//被写入的字节数
    while(true) {
        if ( (n = write(fd, buf, count)) == -1) {
            if (errno == EINTR)
                continue;   //如果是由于信号被中断 则再试一次
            else {
                std::cout << "[Server::Write] write error " << std::endl;
                return -1;
            }
        }
        break;
    }
    return n;
}


