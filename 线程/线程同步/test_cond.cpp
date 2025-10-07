#include<iostream>
#include<pthread.h>
#include<unistd.h>
#include<string>
const int num = 5;

pthread_cond_t gcond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;

void* wait(void* args)
{
    std::string name = static_cast<char*>(args);
    while(true)
    {
        pthread_mutex_lock(&gmutex);
        std::cout << "i am : "<< name << std::endl;
        pthread_cond_wait(&gcond, &gmutex);
        sleep(1);
    }
    return nullptr;
}

int main()
{
    pthread_t threds[num];

    for(int i = 0; i < num;i++)
    {
        char* name = new char[1024];
        snprintf(name, 1024, "thread_%d", i);
        pthread_create(&threds[i], nullptr, wait, (void*)name);
    }


    for(int i = 0; i < num;i++)
    {
        pthread_join(threds[i], nullptr);
    }
}