#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <sys/eventfd.h>

int evfd = -1;

// 唤醒机制是基于文件描述符的.
void *wait(void *args)
{
    std::string name = (const char *)args;
    while (true)
    {
        uint64_t flag = 0; // 必须是8字节的数据
        ssize_t n = ::read(evfd, &flag, sizeof(flag));
        std::cout << name << " 被唤醒..., flag: %d: " << flag << ", errno is : " << errno << std::endl;
    }
}

void *wakeup(void *args)
{
    std::string name = (const char *)args;
    while (true)
    {
        errno++; // 修改errno
        sleep(1);
        std::cout << "wake up one thread, errno : " << errno << std::endl;
        uint64_t flag = 1;
        ::write(evfd, &flag, sizeof(flag));
    }
}

int main()
{
    evfd = ::eventfd(0, 0);
    pthread_t tid1, tid2;
    pthread_create(&tid1, nullptr, wait, (void *)"thread-1");
    pthread_create(&tid2, nullptr, wakeup, (void *)"thread-2");

    pthread_join(tid1, nullptr);
    pthread_join(tid2, nullptr);

    ::close(evfd);
    return 0;
}