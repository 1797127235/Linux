#include<stdio.h>
#include<cstdlib>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
    printf("testexec...begin\n");

    //execlp("python3", "python3", "replace.py", nullptr);
    execl("./replace.py", nullptr);
    printf("testexec...end\n");
}