#pragma once
#include "Socket.hpp"
#include<sys/epoll.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <unordered_map>
using namespace socket_ns;

class EpollServer
{
public:
    EpollServer(uint16_t port):
    _port(port),
    _listensock(std::make_unique<TcpSocket>())
    {   
        _listensock->BuildListenSocket(_port);
        _epollfd = ::epoll_create1(0);
        if(_epollfd == -1)
        {
            throw std::runtime_error("epoll_create1 error");
        }

        //添加监听文件描述符
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _listensock->Sockfd();
        int n = epoll_ctl(_epollfd,EPOLL_CTL_ADD,_listensock->Sockfd(),&ev);

        if(n == -1)
        {
            throw std::runtime_error("epoll_ctl error");
        }
    }

    ~EpollServer()
    {
        if(_epollfd >= 0)
            ::close(_epollfd);
    }

    void Accept()
    {
        InetAddr client;
        SockPtr clientSock = _listensock->Acceptor(&client);
        int sockfd = clientSock->Sockfd();
        if(sockfd > 0)
        {
            std::cout << "Accept connection from " << client.Ip() << ":" << client.Port() << std::endl;
            _clients[sockfd] = clientSock; // 保存 shared_ptr
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = sockfd;
            int n = ::epoll_ctl(_epollfd,EPOLL_CTL_ADD,sockfd,&ev);
            if(n == -1)
            {
                throw std::runtime_error("epoll_ctl error");
            }
        }
    }

    void HandleIO(int fd, uint32_t events) // 就绪文件描述符就绪事件
    {
        if (events & EPOLLIN)
        {
            char buf[4096];
            int n = ::recv(fd, buf, sizeof(buf) - 1, 0); // 用 recv，保留 flags
            if (n == 0)
            {
                int ret = ::epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, nullptr);
                if (ret == -1)
                    throw std::runtime_error("epoll_ctl error (DEL on close)");
                _clients.erase(fd); // 移除 shared_ptr，自动关闭 socket
                return;
            }
            else if (n < 0)
            {
                if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
                    return; // 可重试/等待
                throw std::runtime_error(std::string("recv error: ") + std::strerror(errno));
            }

            buf[n] = '\0';
            // 简单起见，这里不做复杂的 HTTP 解析，只回 200 页面
            // 你也可以根据 buf 里是否包含 "GET /path" 做路由

            // --- 构造 HTML 正文 ---
            std::string body =
                "<!doctype html>\r\n"
                "<html lang=\"zh-CN\">\r\n"
                "<head>\r\n"
                "  <meta charset=\"utf-8\" />\r\n"
                "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\r\n"
                "  <title>你好，世界</title>\r\n"
                "  <style>\r\n"
                "    body{font-family:system-ui,-apple-system,Segoe UI,Roboto,Helvetica,Arial,sans-serif;"
                "         margin:0;display:flex;min-height:100vh;align-items:center;justify-content:center;"
                "         background:#f6f7f9;color:#222}\r\n"
                "    .card{background:#fff;padding:32px;border-radius:16px;box-shadow:0 6px 24px rgba(0,0,0,.08)}\r\n"
                "    h1{margin:0 0 8px;font-size:28px}\r\n"
                "    p{margin:0;opacity:.75}\r\n"
                "  </style>\r\n"
                "</head>\r\n"
                "<body>\r\n"
                "  <div class=\"card\">\r\n"
                "    <h1>HTTP 服务器已就绪</h1>\r\n"
                "    <p>来自 C++/epoll 的问候 </p>\r\n"
                "  </div>\r\n"
                "</body>\r\n"
                "</html>\r\n";

            // --- 构造 HTTP 响应
            std::ostringstream oss;
            oss << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: text/html; charset=utf-8\r\n"
                << "Content-Length: " << body.size() << "\r\n"
                << "Connection: close\r\n"
                << "Server: simple-epoll-server\r\n"
                << "\r\n"
                << body;

            std::string resp = oss.str();

          
            const char* p = resp.data();
            size_t left = resp.size();
            while (left > 0)
            {
                ssize_t sent = ::send(fd, p, left, 0);
                if (sent < 0)
                {
                    if (errno == EINTR) continue;
                    if (errno == EAGAIN || errno == EWOULDBLOCK) { /* 等待下一次可写 */ break; }
                    throw std::runtime_error(std::string("send error: ") + std::strerror(errno));
                }
                p += sent;
                left -= static_cast<size_t>(sent);
            }

            // 简化：主动关连接并从 epoll 移除
            int ret = ::epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, nullptr);
            if (ret == -1)
                throw std::runtime_error("epoll_ctl error (DEL after response)");
            _clients.erase(fd); // 移除 shared_ptr，自动关闭 socket
        }
    }


    void loop()
    {
        int timeout = -1;
        while(true)
        {
            int ret = ::epoll_wait(_epollfd,_events,1024,timeout);
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
                    for(int i = 0;i < ret;i++)
                    {
                        if(_events[i].data.fd == _listensock->Sockfd())
                        {
                            Accept();
                        }
                        else
                        {
                            HandleIO(_events[i].data.fd,_events[i].events);
                        }
                    }
                    break;
            }
        }
    }
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    int _epollfd;
    struct epoll_event _events[1024];
    std::unordered_map<int, SockPtr> _clients; // 保存客户端连接
};