#include<stdio.h>
#include<cstdlib>
#include<unistd.h>
#include<sys/types.h>

int main()
{
    pid_t id = fork();
    if(id > 0)
    {
        while(1)
        {
            printf("i am father process, pid: %d, ppid: %d\n", getpid(), getppid());
            sleep(1);
        }
    }
    else if(id == 0)
    {
        while(1)
        {
            printf("i am child process, pid: %d, ppid: %d\n", getpid(), getppid());
            sleep(1);
        }
    }
    else
    {
        perror("fork error");
        exit(1);
    }
}