#include <cstdio>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    printf("Process is running only me\n");
    sleep(3);
    pid_t pid = fork();
    if (pid == 0) {
        printf("hello from child process\n");
    } else if (pid > 0) {
        printf("hello from parent process\n");
    } else {
        perror("fork failed");
    }
    sleep(5);
    return 0;
}
