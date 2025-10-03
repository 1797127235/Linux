#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int fd[2]; // 用来存放文件描述符的数组
               // fd[0] 是读取端, fd[1] 是写入端

    if (pipe(fd) == -1) {
        perror("pipe error");
        exit(1);
    }

    pid_t id = fork();
    if (id == 0) {
        // --- 子进程：读取数据 ---
        close(fd[1]); // 子进程不写，关闭写入端是个好习惯
        char buffer[100] = {0};
        int n = read(fd[0], buffer, sizeof(buffer) - 1); // 从管道读取数据
        if (n > 0) {
            buffer[n] = '\0'; // 确保字符串以'\0'结尾
            printf("Child received message: %s\n", buffer);
        } else {
            perror("read error");
        }
        close(fd[0]); // 读取完毕，关闭读取端
        
        exit(0);
    } else {
        // --- 父进程：写入数据 ---
        close(fd[0]); // 父进程不读，关闭读取端

        const char *msg = "Hello from parent!";
        printf("Parent sending message...\n");
        int n = write(fd[1], msg, strlen(msg)); // 向管道写入数据
        if (n == -1) {
            perror("write error");
        }
        close(fd[1]); // 写入完毕，关闭写入端
        wait(NULL);   // 等待子进程结束
    }
    return 0;
}