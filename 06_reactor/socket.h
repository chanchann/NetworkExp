/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */
#include <fcntl.h>
namespace sock {

inline void make_nonblocking(int fd) {
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

} // namespace sock
