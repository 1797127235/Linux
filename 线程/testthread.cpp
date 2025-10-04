#include<iostream>
#include<unistd.h>
#include<pthread.h>

void* ThreadRun(void* arg)
{
    while(true)
    {
        std::cout << "thread..." << std::endl;
        sleep(1);
    }
    return nullptr;
}


int main()
{
    pthread_t tid;
    pthread_create(&tid, nullptr, ThreadRun, nullptr);

    while(true)
    {
        std::cout << "main..." << std::endl;
        sleep(1);
    }
    pthread_join(tid, nullptr);
}