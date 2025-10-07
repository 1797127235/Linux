#pragma once
#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<functional>
#include"noCopy.hpp"
#include"Log.hpp"
#include"InetAddr.hpp"
using namespace log_ns;

using func_t = std::function<std::string(const std::string&)>;

class UdpServer:public noCopy
{
public:
    UdpServer(uint16_t port):
        _sockfd(-1),
        _port(port),
        _isrunning(false),
        _func(nullptr)
    {
        _sockfd = ::socket(AF_INET,SOCK_DGRAM,0);
        if(_sockfd < 0)
        {
            std::cerr << "socket error" << std::endl;
            exit(2);
        }

        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_port = htons(_port); //主机转网络字节序
        local.sin_addr.s_addr = INADDR_ANY; //任意地址
        socklen_t len = sizeof(local);
        if(::bind(_sockfd,(struct sockaddr*)&local,len) < 0)
        {
            std::cerr << "bind error" << std::endl;
            exit(3);
        }
    }

    void SetMessageCallback(func_t func)
    {
        _func = func;
    }   

    void Start()
    {
        if(_func == nullptr)
        {
            std::cerr << "must set message callback first" << std::endl;
            return;
        }
        _isrunning = true;
        while(_isrunning)
        {
            char buffer[1024];
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n = recvfrom(_sockfd,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&peer,&len);
            if(n > 0)
            {
                buffer[n] = '\0';
                InetAddr inetaddr(peer);
                std::string msg = buffer;

                std::string response = _func(msg); //业务处理
                
                sendto(_sockfd,response.c_str(),response.size(),0,(struct sockaddr*)&peer,len);
            }
        }
    }
    void Stop()
    {
        _isrunning = false;
    }
    ~UdpServer()
    {
        if(_sockfd >= 0) ::close(_sockfd);
    }



private:
    int _sockfd;
    uint16_t _port;
    bool _isrunning;
    func_t _func; //业务处理函数
};