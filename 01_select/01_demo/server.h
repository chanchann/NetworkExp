#ifndef SERVER_H
#define SERVER_H

class Server {
public:
    Server(int port = 10000, int port = 1024);
    ~Server();

    void run();

private:
    bool InitSocket();
    void AcceptConnection();
    void echo(int fd);

private:
    int _sockfd;
    int _port;
    int _backlog;


};

#endif // SERVER_H