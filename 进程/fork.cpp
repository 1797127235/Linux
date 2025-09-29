#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <cstdio>  // 引入 cstdio 头文件

void test1()
{
    printf("Process is running only me\n");  // 使用 printf
    sleep(3);  // sleep 需要unistd.h
    pid_t pid = fork();  // 创建子进程
    if (pid == 0) {
        // 子进程
        printf("hello from child process\n");
    } else if (pid > 0) {
        // 父进程
        printf("hello from parent process\n");
    } else {
        // fork 失败
        perror("fork failed");
    }
    sleep(5);  // 等待子进程完成
}

void test2()
{
    //一次创建多个子进程
    const int num = 5;
    auto RunChild = [](){
        while(1)
        {
            printf("i am child process, pid = %d ppid = %d\n", getpid(), getppid());
            sleep(1);
        }
    };

    for(int i = 0; i < num; ++i)
    {
        pid_t pid = fork();
        if(pid == 0)
        {
            RunChild();
        }
    }

    while(1)
    {
        printf("i am parent process, pid = %d\n ppid = %d\n", getpid(), getppid());
        sleep(1);
    }
}

int main()
{
    return 0;
}
