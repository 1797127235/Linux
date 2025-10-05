#include "Thread.hpp"

void print()
{
    for(int i = 0; i < 10; i++)
    {
        std::cout << "i = " << i << std::endl;
    }
}

int main()
{
    Thread t1(print);
    t1.join();
    return 0;
}