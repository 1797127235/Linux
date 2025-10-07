#pragma once 
#include<pthread.h>

class LockGuard
{
public:
    LockGuard(pthread_mutex_t* lock)
        :_lock(lock)
    {
        pthread_mutex_lock(_lock);
    }
    ~LockGuard()
    {
        pthread_mutex_unlock(_lock);
    }
private:
    pthread_mutex_t* _lock;
};