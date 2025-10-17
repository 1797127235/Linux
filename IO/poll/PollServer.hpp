#pragma once
#include "Socket.hpp"
#include <memory>
#include<sys/poll.h>

class PollServer
{
    const static int gnum = 1024;
    const static int gdefaultfd = -1;
public:
    PollServer(uint16_t port,size_t num = gnum) :
     _port(port),
     _listensock(std::make_unique<TcpSocket>()),
     _num(num)
    {
        _listensock->BuildListenSocket(_port);
    }
    ~PollServer()
    {
        
    }

    void Init()
    {
        for(int i = 0; i < _num; i++)
        {
            _pollfds[i].fd = gdefaultfd;
            _pollfds[i].events = 0;
            _pollfds[i].revents = 0;
        }
        _pollfds[0].fd = _listensock->Sockfd();
        _pollfds[0].events = POLLIN;
    }

    void Accept()
    {
        InetAddr client;
        int sockfd = _listensock->Acceptor(&client)->Sockfd();

        if(sockfd > 0)
        {
            bool flag = false;
            for(int pos = 1;pos < _num;pos++)
            {
                if(_pollfds[pos].fd == gdefaultfd)
                {
                    flag = true;
                    _pollfds[pos].fd = sockfd;
                    _pollfds[pos].events = POLLIN;
                    break;
                }
            }
            if(!flag)
            {
                close(sockfd);
            }
        }
        
    }

    void HandleIO(int i)
    {
        int fd = _pollfds[i].fd;
        int n = recv(fd,nullptr,0,0);
        if(n == 0)
        {
            close(fd);
            _pollfds[i].fd = gdefaultfd;
            _pollfds[i].events = 0;
            _pollfds[i].revents = 0;
        }
        else if(n < 0) //错误
        {
            if(errno == EINTR)
                return;
            close(fd);
            _pollfds[i].fd = gdefaultfd;
            _pollfds[i].events = 0;
            _pollfds[i].revents = 0;
        }
        else
        {
            //todo
            send(fd,"hello",5,0);
        }
    }

    void HandlerEvent(int ret)
    {
        for(int i = 0; i < _num; i++)
        {
            if(_pollfds[i].revents & POLLIN) //有数据可读
            {
                if(_listensock->Sockfd() == _pollfds[i].fd)
                {
                    Accept();
                }
                else
                {
                    HandleIO(i);
                }
            }
        }
    }

    void loop()
    {
        int timeout = -1;
        while(true)
        {
            int ret = poll(_pollfds,_num,timeout);
            switch(ret)
            {
                case 0:
                    std::cout << "timeout" << std::endl;
                    break;
                case -1:
                    if(errno == EINTR)
                        continue;
                    break;
                default:
                    HandlerEvent(ret);
                    break;
            }
        }
    }
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    size_t _num;
    pollfd _pollfds[_num];
}