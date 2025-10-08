#pragma once
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string>
#include<unistd.h>
#include<cstring>
#include<functional>
#include<memory>
#include<thread>
#include<iostream>
#include"Log.hpp"
#include"InetAddr.hpp"
#include "Command.hpp"

using namespace log_ns;

using task_t = std::function<void(const char*, int)>;
class TcpServer
{
public:
    TcpServer(uint16_t port = 8888):
        _listensock(-1),
        _port(port),
        _isrunning(false),
        _task(nullptr)
    {
        _listensock = socket(AF_INET,SOCK_STREAM,0);
        if(_listensock < 0)
        {
            LOG(ERROR,"socket error");
            exit(2);
        }
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;
        if(::bind(_listensock,(struct sockaddr*)&local,sizeof(local)) < 0)
        {
            LOG(ERROR,"bind error");
            exit(2);
        }

        if(listen(_listensock,8) < 0)
        {
            LOG(ERROR,"listen error");
            exit(2);
        }
    }

    ~TcpServer()    
    {
        if(_listensock >= 0) ::close(_listensock);
    }

    void serverice(int clientfd,InetAddr& peer)
    {
        while(true)
        {
            char buffer[1024];
            int n = ::read(clientfd,buffer,sizeof(buffer) - 1);
            if(n <= 0)
            {
                break;
            }
            buffer[n] = 0;
            std::cout << "read: "<<buffer<<std::endl;

            //执行回调
            _task(buffer, clientfd);
        }
    }

    void SetTask(task_t task)
    {
        _task = task;
    }

    void Start()
    {
        if(_task == nullptr)
        {
            LOG(ERROR,"no task");
            return;
        }
        _isrunning = true;
        while(_isrunning)
        {
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            int clientfd = accept(_listensock,(struct sockaddr*)&client,&len);
            if(clientfd < 0)
            {
                LOG(WARNING,"accept error");
                continue;
            }

            InetAddr peer(client);

            auto func = std::bind(&TcpServer::serverice,this,clientfd,peer);

            std::thread t(func); //创建一个新的线程
            t.detach();
        }
    }

    void Stop()
    {
        _isrunning = false;
    }
private:
    int _listensock;
    int _port;
    bool _isrunning;
    task_t _task;
};

