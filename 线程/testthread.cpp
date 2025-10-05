#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include<cstring>

void* ThreadRun(void* arg)
{
    char* name = (char*)arg;
    int cnt = 5;
    while(cnt--)
    {
        std::cout << name << " is running..." << std::endl;
        sleep(1);
    }
    return (void*)111;
}


int main()
{
    pthread_t tid;
    int n = pthread_create(&tid, nullptr, ThreadRun, (void *)"thread1");
    if(n != 0)
    {
        std::cout << "pthread_create error: " << strerror(n) << std::endl;
        return 1;
    }


    int res;
    n = pthread_join(tid, (void**)&res); //线程等待
    if(n == 0)
    {
        std::cout << "pthread_join success..." << std::endl;
        std::cout << "Thread return value: " << res << std::endl;
    }
    else
    {
        std::cout << "pthread_join error: " << strerror(n) << std::endl;
    }

    return 0;
}