#pragma once
#include"Socket.hpp"
#include<sys/select.h>
#include<cerrno>

class SelectServer
{
    const static int gnum = sizeof(fd_set) * 8;
    const static int gdefaultfd = -1;
public:
    SelectServer(uint16_t port) :
     _port(port),
     _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildListenSocket(_port);
    }
    ~SelectServer()
    {}

    void Init()
    {
        for(int i = 0;i < gnum;i++)
            fd_array[i] = gdefaultfd;
        
        //默认直接添加
        fd_array[0] = _listensock->Sockfd();
    }

    void Accept()
    {
        InetAddr client;
        int newfd = _listensock->Acceptor(&client)->Sockfd();

        if(newfd > 0)
        {
            bool flag = false;

            for(int i = 0;i < gnum;i++)
            {
                if(fd_array[i] == gdefaultfd)
                {
                    fd_array[i] = newfd;
                    flag = true;
                    break;
                }
            }
        }

        if(!flag)
        {
            close(newfd);
        }
        
    }

    void HandleIO(int i)
    {
        int fd = fd_array[i];
        int n = recv(fd,nullptr,0,0);
        if(n == 0)
        {
            close(fd);
            fd_array[i] = gdefaultfd;
        }
        else if(n < 0)
        {
            if(errno == EINTR)
                return;
            close(fd);
            fd_array[i] = gdefaultfd;
        }
        else
        {
            //todo
            send(fd,"hello",5,0);
        }
    }

    void HandlerEvent(fd_set& rfds)
    {
        for(int i = 0;i < gnum;i++)
        {
            if(FD_ISSET(fd_array[i],&rfds))
            {
                if(fd_array[i] == _listensock->Sockfd())
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
        while(true)
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            int max_fd = -1;
            for(int i = 0;i < gnum;i++)
            {
                if(fd_array[i] != gdefaultfd)
                {
                    FD_SET(fd_array[i],&rfds);
                    max_fd = std::max(max_fd,fd_array[i]);
                }
            }

            struct timeval timeout = {30,0};
            int ret = select(max_fd + 1,&rfds,nullptr,nullptr,&timeout);
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
                    HandlerEvent(rfds);
                    break;
            }
        }
    }
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;

    //辅助数组，保存所有合法fd
    int fd_array[gnum];
};
