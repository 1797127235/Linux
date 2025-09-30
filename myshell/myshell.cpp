#include<unistd.h>
#include<cstring>
#include<iostream>
#include<sstream>
#include<sys/wait.h>

const char ZERO = '\0';
const char* SEP = " ";
const int NUM = 32;
char* argv[NUM];
char cwd[1024];
int lastcode = 0;


const char* GetHome()
{
    const char* home = getenv("HOME");
    if(home == nullptr) return "/";
    return home;
}

const char* GetUserName()
{
    const char* user = getenv("USER");
    if(user == nullptr) return "unknown";
    return user;
}



//输出命令行
void MakeCommandLine()
{
    const char* home = GetHome();
    const char* user = GetUserName();
    std::cout << user << "@" << home << ":~$ " ;
    std::cout.flush();
}

int GetUserCommand(char* command,int size)
{
    char* s = fgets(command, size, stdin);
    if(s == nullptr) return -1;
    int len = strlen(command);
    command[len-1] = ZERO; 
    return len;
}

void  SplitCommand( char command[],int size)
{
    argv[0] = strtok(command,SEP);
    int idx = 1;
    while((argv[idx++] = strtok(nullptr,SEP)));

}

void ExcuteCommand()
{
    pid_t pid = fork();
    if(pid < 0) exit(1);
    else if(pid == 0)
    {
        execvp(argv[0],argv);
        exit(1);
    }
    else
    {
        int status = 0;
        pid_t rid = waitpid(pid,&status,0);
        if(rid == pid)
        {
            lastcode = WEXITSTATUS(status);
        }
    }
}

void Cd()
{
    const char *path = argv[1];
    if(path == NULL) path = GetHome();
    // path 一定存在
    chdir(path);

    // 刷新环境变量
    char temp[1024+10];
    getcwd(temp, sizeof(temp));
    snprintf(cwd, sizeof(cwd), "PWD=%.*s", (int)sizeof(cwd)-5, temp);
    putenv(cwd); // OK
}


int CheckBuildin()
{
    int yes = 0;
    const char * cmd = argv[0];
    if(strcmp(cmd,"cd") == 0)
    {
        yes = 1;
        Cd();
    }
    else if(strcmp(cmd,"echo") == 0 && strcmp(argv[1],"$?") == 0)
    {
        yes = 1;
        std::cout << lastcode << std::endl;
        lastcode = 0;
    }
    return yes;
}







int main()
{
    int quit = 0;
    while(!quit)
    {
        MakeCommandLine();
        char command[1024];
        int len = GetUserCommand(command, sizeof(command));
        if(len <= 0) return 1;
        SplitCommand(command, len);
        int isbuildin = CheckBuildin();
        if(isbuildin) continue;
        ExcuteCommand();



    }
}