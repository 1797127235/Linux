#pragma once
#include"Socket.hpp"
#include"Connection.hpp"
#include"InetAddr.hpp"
#include<memory>

using namespace socket_ns;

class Listener
{
public:
    Listener() = default;
    ~Listener() = default;

    Listener(uint16_t port):
        _port(port),
        _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildListenSocket(port);
    }

    int listensock() const
    {
        return _listensock->Sockfd();
    }


    void Acceptor(Connection* conn) //非阻塞收取链接  ET模式  必须循环获取！！！！！
    {
        std::cout << "Acceptor called" << std::endl;
        while(true)
        {
            errno = 0;
            InetAddr addr;
            int code = 0;
            int sockfd = _listensock->Acceptor(&addr,&code);
            if(sockfd > 0)
            {
                std::cout << "Accepted new connection, sockfd: " << sockfd << std::endl;
                conn->_R->AddConnection(sockfd,EPOLLIN|EPOLLET,addr,ConnectionType::NormalConnection);
            }
            else
            {
                if(code == EWOULDBLOCK)
                {
                    break;
                }
                else if(code == EINTR)
                {
                   continue;
                }
                else
                {
                    break;
                }
            }

        }
    }



private:
    uint16_t _port;
    std::unique_ptr<TcpSocket> _listensock;
};