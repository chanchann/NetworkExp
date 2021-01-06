#include <iostream>
#include "server.h"

int main() {
    Server server(10001);
    server.run();
    return 0;
}