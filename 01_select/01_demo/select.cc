#include "select.h"


Select::Select() {
    // init
    zero();
    update();
}

void Select::set(int fd) {
    FD_SET(fd, &_allset); 
    if(_maxfd < fd) 
        _maxfd = fd;
}

int Select::select() {
    int nready = ::select(_maxfd+1, &_rset, nullptr, nullptr, nullptr);
    if(nready < 0) 
        std::cout << "[Select::select Error] Select Error" << std::endl;
    return nready;
}

bool Select::isFocus(int fd) {
    int ret = FD_ISSET(fd, &_rset);
    if(ret == 0) return false;
    else return true;
}

void Select::add(int fd) {
    if(_clients.size() >= 1024) {
        std::cout << "Too many clients!!!" << std::endl;
        return;
    }
    auto it = std::find(_clients.begin(), _clients.end(), fd);
    if (it != _clients.end()) {  // find
        std::cout << "[ Select::add ] This fd is in it" << std::endl;
    }
    else {   // not find
        _clients.push_back(fd);
        set(fd);
    }
}

void Select::clear(int fd) {
    close(fd);
    FD_CLR(fd, &_allset);
    _clients.erase(std::find(_clients.begin(),
                            _clients.end(),
                            fd));
}

int Select::getClient(int i) const {
    return _clients.at(i);
}

size_t Select::getClientSize() const {
    return _clients.size();
}
