/*
 * @Author: Shiyu Yi
 * @Github: https://github.com/chanchann
 */

#include "server.h"
#include <iostream>
// 观察实验现象，这是由于select 默认LT吗，还需要再梳理

int main() {
    Server server;
    server.run();
    
}