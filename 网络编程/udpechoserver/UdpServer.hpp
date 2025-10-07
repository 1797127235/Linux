#pragma once
#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include"noCopy.hpp"
#include"Log.hpp"
using namespace log_ns;

class UdpServer:public noCopy
{
public:
    UdpServer(const char*ip,uint16_t port):
        _sockfd(-1),
        _port(port),
        _ip(ip),
        _isrunning(false)
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
        local.sin_addr.s_addr = inet_addr(_ip.c_str());
        socklen_t len = sizeof(local);
        if(::bind(_sockfd,(struct sockaddr*)&local,len) < 0)
        {
            std::cerr << "bind error" << std::endl;
            exit(3);
        }
    }

    void Start()
    {
        _isrunning = true;
        while(_isrunning)
        {
            char buffer[1024];
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n = recvfrom(_sockfd,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&peer,&len);
            if(n > 0)
            {
                buffer[n] = 0;
                std::cout << "client# " << buffer << std::endl;
                sendto(_sockfd,buffer,strlen(buffer),0,(struct sockaddr*)&peer,len);
            }
        }
    }
    void Stop()
    {
        _isrunning = false;
    }
    ~UdpServer()
    {
        close(_sockfd);
    }



private:
    int _sockfd;
    uint16_t _port;
    std::string _ip;
    bool _isrunning;
};