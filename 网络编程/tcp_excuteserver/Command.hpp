#pragma once
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include "Log.hpp"
using namespace log_ns;
class Command
{
public:
    Command() = default;

    virtual ~Command() = default;

    // void Excute(const char* cmd, int clientfd)
    // {
    //     if(cmd == nullptr) return;
    //     std::istringstream iss(cmd);
    //     std::vector<std::string> parts;
    //     for(std::string t;iss>>t;) parts.push_back(t);
    //     if(parts.empty()) return;

    //     std::vector<char*> argv;
    //     argv.reserve(parts.size() + 1);

    //     for(auto &s : parts) argv.push_back(const_cast<char*>(s.c_str()));
    //     argv.push_back(nullptr);

    //     pid_t pid = fork();
    //     if(pid ==0)
    //     {
    //         dup2(clientfd,STDOUT_FILENO);
    //         dup2(clientfd,STDERR_FILENO);
    //         execvp(argv[0],argv.data());
    //         exit(2);
    //     }
    //     else if(pid > 0)
    //     {
    //         close(clientfd);
    //         int status = 0;
    //         if(waitpid(pid,&status,0) < 0)
    //         {
    //             perror("waitpid error");
    //             exit(2);
    //         }

    //         const char* ok = "OK\n";
    //         ::write(clientfd, ok, strlen(ok));
    //     }
    //     else
    //     {
    //         perror("fork error");
    //         exit(2);
    //     }
    // }

    void Excute(const char* cmd,int clientfd)
    {
        if(!cmd) return ;
        std::string result;
        FILE* fp = popen(cmd,"r");
        if(!fp) return ;

        char line[1024];
        while(fgets(line,sizeof(line),fp))
        {
            result += line;
        }

        int n = ::write(clientfd,result.c_str(),result.size());
        if(n < 0)
        {
            LOG(ERROR,"write error");
            return;
        }
        pclose(fp);
    }
};