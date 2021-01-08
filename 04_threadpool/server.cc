/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */
#include "server.h"

bool Server::InitSocket()  {
    _listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(_port);

    int opt = 1;
    setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int rt1 = bind(_listenfd, 
                reinterpret_cast<struct sockaddr *>(&server_addr),
                sizeof(server_addr));
    if (rt1 < 0) {
        std::cout << "bind failed " << std::endl;
        return false;
    }

    int rt2 = listen(_listenfd, _backlog);
    if (rt2 < 0) {
        std::cout << "listen failed " << std::endl;
        return false;
    }
    signal(SIGPIPE, SIG_IGN);
    return true;
}

void Server::AcceptConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int fd = accept(_listenfd, 
                reinterpret_cast<struct sockaddr *>(&client_addr),
                &client_addr_len);
    if(fd < 0) {
        std::cout << "Accept Error" << std::endl;
        exit(1);  
    } else {
        _blockQue->push(fd);
    }
}

void Server::LoopEcho(int fd) {
    char outbuf[MAX_LINE + 1];
    size_t outbuf_used = 0;
    ssize_t result;
    while (1) {
        char ch;
        result = recv(fd, &ch, 1, 0);

        //断开连接或者出错
        if (result == 0) {
            break;
        } else if (result == -1) {
            std::cout << "Read Error" << std::endl;
            break;
        }

        if (outbuf_used < sizeof(outbuf)) {
            outbuf[outbuf_used++] = toupper(ch);
        }

        if (ch == '\n') {
            send(fd, outbuf, outbuf_used, 0);
            outbuf_used = 0;
            continue;
        }
    }
}

void Server::thrdRun() {
    while(true) {
        int fd = _blockQue->pop();
        std::cout << "get fd in thread, fd == " << fd 
            <<  " tid == " << std::this_thread::get_id() << std::endl; 
        LoopEcho(fd);
    } 
}

Server::Server(int port, int thrdNum) : 
        _port(port),
        _threadList(thrdNum),
        _blockQue(make_unique<BlockQue>()) {
    if(!InitSocket()) {
        std::cout << " Init socket fails" << std::endl;
        return;
    }
}

Server::~Server() {
    close(_listenfd);
}

void Server::run() {
    std::cout << "Running ..." << std::endl;
    for(size_t i = 0; i < _threadList.size(); i++) {
        _threadList[i] = std::thread(&Server::thrdRun, this);
    }
    std::for_each(_threadList.begin(), _threadList.end(),
                    std::mem_fn(&std::thread::detach));
    while(true) {
        AcceptConnection();
    }
}
