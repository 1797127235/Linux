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
#include"Log.hpp"
#include"InetAddr.hpp"
#include"ThreadPool.hpp"
#include"Lockguard.hpp"

using namespace log_ns;

class TcpServer
{
public:
    TcpServer(uint16_t port = 8888):
        _listensock(-1),
        _port(port),
        _isrunning(false)
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
            LOG(INFO,"read %d bytes from %s:%d\n",n,peer.Ip().c_str(),peer.Port());
            if(n <= 0)
            {
                break;
            }

            buffer[n] = 0;
            int m = ::write(clientfd,buffer,n);
            if(m <= 0)
            {
                break;
            }  
        }
    }

    void Start()
    {
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
            LOG(INFO,"new connection from %s:%d",peer.Ip().c_str(),peer.Port());
            serverice(clientfd,peer);
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
};

