#include<pthread.h>
#include<iostream>
#include<string>
#include<functional>
using id = pthread_t;

using func_t = std::function<void()>;

template<typename Fn, typename... Args>
class Thread
{
public:
    Thread() = default;
    Thread(Fn fn, Args... args)
    {
        
    }


private:
    pthread_t _tid;
    bool _isrunning;
};