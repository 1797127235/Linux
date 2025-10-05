#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include"Lockguard.hpp"

int ticket = 10000;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void* route(void* arg)
{
    std::string name = (char*)arg;
    while(true)
    {
        LockGuard guard(&lock);
        if(ticket > 0)
        {
            usleep(1000);
            std::cout << name << " is saling ticket: " << ticket-- << std::endl;
        }
        else
        {
            break;
        }
    }
    return nullptr;
}



int main()
{
    pthread_t t1, t2, t3;
    pthread_create(&t1, nullptr, route, (void *)"thread1");
    pthread_create(&t2, nullptr, route, (void *)"thread2");
    pthread_create(&t3, nullptr, route, (void *)"thread3");

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);

    return 0;
}