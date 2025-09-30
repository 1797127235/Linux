#include<stdio.h>
#include<cstdlib>
#include<unistd.h>

int g_val = 100;

//子进程改变变量，发现地址一样  虚拟地址
int main()
{
    printf("father is running ,pid: %d, ppid: %d\n", getpid(), getppid());
    pid_t id = fork();
    if(id == 0)
    {
        int cnt = 0;
        while(1)
        {
            printf("i am child process, pid: %d, ppid: %d, g_val: %d, &g_val: %p\n", getpid(), getppid(), g_val, (void*)&g_val);
            sleep(1);
            if(cnt == 5)
            {
                g_val = 200;
            }              
            cnt ++;
        }
    }
    else
    { 
        while(1)
        {
            printf("i am father process, pid: %d, ppid: %d, g_val: %d, &g_val: %p\n", getpid(), getppid(), g_val, (void*)&g_val);
            sleep(1);
            // g_val++;
        }
    }
    return 0;
}