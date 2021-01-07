#include <iostream>
#include <thread>

int another_shared = 0;

void thread_run(int &cal) {
    std::cout << "tid == " << std::this_thread::get_id() << std::endl;
    for(int i = 0; i < 1000; i++) {
        cal += 1;
        another_shared += 1;
    }
}

int main() {
    int calculator = 0;
    std::thread t1(thread_run, std::ref(calculator));
    std::thread t2(thread_run, std::ref(calculator));

    t1.join();
    t2.join();
    std::cout << "calculator is " << calculator << std::endl;
    std::cout << "another_shared is " << another_shared << std::endl;
    return 0;
}