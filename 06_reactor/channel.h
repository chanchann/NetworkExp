/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */


#ifndef CHANNEL_H 
#define CHANNEL_H 
#include <functional>

using Callback = std::function<void()>;
class Channel {
public:
    Channel();
    ~Channel();

public:
    static const int EVENT_TIMEOUT = 0x01;
    static const int EVENT_READ = 0x02;
    static const int EVENT_WRITE = 0x04;
    static const int EVENT_SIGNAL = 0x08;
private:
    int fd_;
    int events_;  // 表示event类型
    Callback readcb;
    Callback writecb;
    // callback data
};







#endif	// CHANNEL_H