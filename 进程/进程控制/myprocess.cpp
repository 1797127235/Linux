#include<stdio.h>
#include<cstdlib>
#include<unistd.h>

int main()
{
    printf("myprocess is running ,pid: %d, ppid: %d\n", getpid(), getppid());
    sleep(5);
    return 100;
}