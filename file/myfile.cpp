#include<cstdio>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>

int main()
{
    // int fd = open("log.txt",O_WRONLY | O_CREAT,0666);
    // printf("fd=%d\n",fd);
    // if(fd < 0) return 1;
    write(1,"hello\n",6);

    return 0;
}