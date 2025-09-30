#include<stdio.h>
#include<cstdlib>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
void run()
{
    int cnt = 0;
    while(cnt < 5)
    {
        printf("i am child process, pid: %d, ppid: %d\n", getpid(), getppid());
        sleep(1);
        cnt++;
    }
}

void Doother()
{
    printf("do other things in parent process, pid = %d\n", getpid());
    sleep(1);
}

int main()
{
    pid_t pid = fork();
    if(pid > 0)
    {

        printf("i am parent process, pid = %d\n ppid = %d\n", getpid(), getppid());
        // sleep(10);
        // int status;
        // //pid_t id = waitpid(pid, &status, WNOHANG); //非阻塞等待指定子进程退出
        // pid_t id = waitpid(-1, &status, 0); //阻塞等待任意一个子进程退出
        // //wait(&status); //阻塞等待任意一个子进程退出
        // printf("child process id = %d\n", id);
        // if(WIFEXITED(status))
        // {
        //     printf("child process exit code = %d\n", WEXITSTATUS(status));
        // }
        // else //子进程不是正常退出的
        // {
        //     printf("child process terminated abnormally\n");
        // }


        //进行非阻塞等待
        while(1)
        {
            int status = 0;
            pid_t id = waitpid(-1, &status, WNOHANG);
            if(id == 0)
            {
                printf("child process is running, father wait\n");
                Doother();
                usleep(500*1000); //休息500ms
            }
            else if(id > 0) //等待成功了
            {
                if(WIFEXITED(status))
                {
                    printf("child process exit code = %d\n", WEXITSTATUS(status));
                }
                else //子进程不是正常退出的
                {
                    printf("child process terminated abnormally\n");
                }
                break;
            }
            else
            {
                perror("waitpid failed");
                return 1;
            }
        }
    }
    else if(pid == 0) //子进程
    {
        run();
    }
    else
    {
        perror("fork failed");
        return 1;
    }

}