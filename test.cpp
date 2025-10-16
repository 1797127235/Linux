#include <iostream>
#include <thread>
#include <unistd.h>

int gval = 1;

void change() {
    sleep(1);
    gval = 0;
}

int main() {
    std::thread t1(change);
    while (gval) { }  
    std::cout << "gval changed!\n";
    t1.join();
}
