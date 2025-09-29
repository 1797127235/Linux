#include <iostream>
#include <unistd.h>

//验证运行状态
//此时其实大部分看见的是睡眠状态，因为进程大部分时间都在IO
void test1()
{
    while (true)
    {
        std::cout << "I am process, pid = " << getpid() << std::endl;
        sleep(1);
    }
}

//验证僵尸进程 子进程先退了，父进程不回收
//操作系统的资源有限，僵尸进程会造成内存泄漏问题
void test2()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        int cnt = 5;
        while(cnt--)
        {
            std::cout << "I am child process, pid = " << getpid() << std::endl;
            sleep(1);
        }
    }
    else
    {
        while (true)
        {
            std::cout << "I am parent process, pid = " << getpid() << std::endl;
            sleep(1);
        }
    }
}

//孤儿进程 父进程先退了，子进程被init进程收养
void test3()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        while(1)
        {
            std::cout << "I am child process, pid = " << getpid() << " ppid = " << getppid() << std::endl;
            sleep(1);
        }
    }
    else
    {
        int cnt = 5;
        while(cnt--)
        {
            std::cout << "I am parent process, pid = " << getpid() << std::endl;
            sleep(1);
        }

    }
}

//阻塞进程
void test4()
{
    int x;
    scanf("%d", &x); //阻塞等待输入
    std::cout << "I am process, pid = " << getpid() << std::endl;
    sleep(1);
}

int main()
{
    test3();
    return 0;
}