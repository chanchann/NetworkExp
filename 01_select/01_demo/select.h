#ifndef SELECT_H
#define SELECT_H
#include <sys/select.h>
#include <unistd.h> // close
#include <iostream>
#include <vector>
#include <algorithm>

class Select {
public:
    Select();
    ~Select() = default;
    void zero() { FD_ZERO(&_allset); }
    void update() { _rset = _allset; }
    int select();
    bool isFocus(int fd);
    void set(int fd);
    void add(int fd);
    void clear(int fd);
    int getClient(int i) const;
    size_t getClientSize() const;
private:
    fd_set _rset;
    fd_set _allset;
    int _maxfd;
    std::vector<int> _clients;
};
#endif // SELECT_H