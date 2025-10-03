#include<iostream>
#include<string>
#include<vector> 
#include<unistd.h>
#include<sys/types.h>
#include<string>
#include<sys/wait.h>
#include"Task.hpp"

class Channel
{
public:
    Channel(int wfd,pid_t id,const std::string& name):
    _wfd(wfd),
    _subprocessid(id),
    _name(name)
    {

    }
    ~Channel()
    {

    }
    int GetWfd()
    {
        return _wfd;
    }

    void CloseChannel()
    {
        cloase(_wfd);
    }

    void Wait()
    {
        pid_t id = waitpid(_subprocessid,nullptr,0);
    }

private:
    int  _wfd;
    pid_t _subprocessid; //子进程id
    std::string _name;
}

void work()
{
    while(true)
    {
        int idx = -1;
        int n = read(0,&idx,sizeof(idx));
        if(n == sizeof(idx))
        {
            ExcuteTask(idx);
        }
        else if(n == 0)
        {
            break;
        }
    }
}

int NextChannel(int channelnum)
{
    static int next = 0;
    int channel = next;
    next ++;
    next %= channelnum;
    return channel;
}


void CreateChannelAndSub(std::vector<Channel>* channels,int num,task_t task)
{
    for(int i = 0; i < nums; i++)
    {
        int pipefd[2] = {0};
        int n = pipe(pipefd);
        if(n < 0)
        {
            perror("pipe error");
            exit(1);
        }


        pid_t id = fork();

        if(id == 0)
        {
            if(!channels->empty())
            {
                for(auto& channel : *channels)
                {
                    channel.CloseChannel(); //关闭历史上的写端 保证每个管道只有一个写端一个读端
                }
            }
            close(pipefd[1]);
            dup2(pipefd[0],0);
            // work();
            task();
            close(pipefd[0]);
            exit(0);
        }

        close(pipefd[0]);
        channels->emplace_back(pipefd[1],id,"subprocess_" + std::to_string(i));
        close(pipefd[1]); 
    }

}

void SendTaskCommand(Channel &channel, int taskcommand)
{
    write(channel.GetWfd(), &taskcommand, sizeof(taskcommand));
}

void CtrlProcess(vector<Channel>& channels)
{
    //通过信道和子进程进行通信
    while(true)
    {
        // a.选择一个任务
        int task_idx = SlectTaks();

        //b.选择一个信道和进程

        int channel = NextChannel(num);

        // c.发送任务
        SendTaskCommand(channels[channel],task_idx);
    }

}

void CleanUp(std::vector<Channel>& channels)
{
    for(auto& channel : channels)
    {
        channel.CloseChannel();
    }
    //注意必须这么写！！！！！
    for(auto& channel : channels)
    {
        channel.Wait();
    }
}


int main(int argc, char* argv[])
{
    if(argc !=2)
    {
        std::cout << "Usage: " << argv[0] << " processnum" << std::endl;
        exit(1);
    }
    int num = std::stoi(argv[1]);


    //创建信道和子进程
    std::vector<Channel> channels;
    CreateChannelAndSub(&channels,num,work);
    LoadTask();  //加载任务

    CtrlProcess(channels);  //控制进程
    //回收管道和子进程



}