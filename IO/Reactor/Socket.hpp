#pragma once
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cstring>
#include<string>
#include<memory>
#include<cstdlib>
#include "InetAddr.hpp"
#include"Comm.hpp"


const int DEFAULT_BACKLOG = 8;

namespace socket_ns
{
    //模板方法方式封装
    class Socket;
    using SockPtr = std::shared_ptr<Socket>;
    class Socket
    {
    public:
        virtual ~Socket() = default;
        virtual void CreateSocketOrDie() = 0;
        virtual void CreateBindOrDie(uint16_t port) = 0;
        virtual void CreateListenOrDie(int backlog = DEFAULT_BACKLOG) = 0;
        virtual int Acceptor(InetAddr* cliaddr,int* code) = 0;
        virtual bool Connector(const std::string& ip, uint16_t port) = 0;
        virtual int Sockfd() const = 0;
        virtual void Close() = 0;

        virtual ssize_t Send(const std::string& message) = 0;
        virtual ssize_t Recv(std::string* message) = 0;
    public:
        void BuildListenSocket(uint16_t port,int backlog = DEFAULT_BACKLOG)
        {
            CreateSocketOrDie();
            CreateBindOrDie(port);
            CreateListenOrDie(backlog);
        }

        bool BuildClientSocket(const std::string& ip, uint16_t port)
        {
            CreateSocketOrDie();
            return Connector(ip,port);
        }
    };

    class TcpSocket:public Socket
    {
    public:
        TcpSocket() : _sockfd(-1), _is_server(false){}
        TcpSocket(int sockfd) : _sockfd(sockfd), _is_server(false){}
        ~TcpSocket() override { Close(); }
        void CreateSocketOrDie() override
        {
            _sockfd = ::socket(AF_INET,SOCK_STREAM,0);
            if(_sockfd < 0)
            {
                perror("socket");
                exit(2);
            }
            // 只对服务器套接字设置非阻塞
            if(_is_server) {
                SetNonBlock(_sockfd);
            }
        }
        void CreateBindOrDie(uint16_t port) override
        {
            _is_server = true;  // 标记为服务器套接字
            int opt = 1;
            (void)::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            struct sockaddr_in local{};
            local.sin_family = AF_INET;
            local.sin_port = htons(port);
            local.sin_addr.s_addr = INADDR_ANY;
            if(bind(_sockfd,(struct sockaddr*)&local,sizeof(local)) < 0)
            {
                perror("bind");
                exit(2);
            }
        }
        void CreateListenOrDie(int backlog = DEFAULT_BACKLOG) override
        {
            // 确保监听套接字为非阻塞，避免在 ET 模式下 accept 循环阻塞
            SetNonBlock(_sockfd);
            if(listen(_sockfd,backlog) < 0)
            {
                perror("listen");
                exit(2);
            }
        }

        int Acceptor(InetAddr* cliaddr,int* code) override
        {
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            int sockfd = ::accept(_sockfd,(struct sockaddr*)&client,&len);
            *code = errno;
            if(sockfd < 0)
            {
                return -1;
            }
            SetNonBlock(sockfd);
            *cliaddr = InetAddr(client);
            return sockfd;
        }

        void Close() override 
        {
            if (_sockfd >= 0) { ::close(_sockfd); _sockfd = -1; }
        }

        int Sockfd() const override
        {
            return _sockfd;
        }

        bool Connector(const std::string& ip, uint16_t port) override
        {
            sockaddr_in remote{};
            remote.sin_family = AF_INET;
            remote.sin_port   = htons(port);
            if (::inet_pton(AF_INET, ip.c_str(), &remote.sin_addr) != 1) {
                std::fprintf(stderr, "inet_pton failed for %s\n", ip.c_str());
                std::exit(2);
            }
            if (::connect(_sockfd, reinterpret_cast<sockaddr*>(&remote), sizeof(remote)) < 0) {
                perror("connect"); 
                std::exit(2);
            }
            return true;
        }

        ssize_t Send(const std::string& message) override
        {
            return ::send(_sockfd,message.c_str(),message.size(),0);
        }

        // ssize_t Recv(std::string* message) override {
        //     char buffer[2048];
        //     ssize_t n = ::recv(_sockfd, buffer, sizeof(buffer), 0);
        //     if (n < 0) { perror("recvfrom/recv"); std::exit(2); }
        //     if (n == 0) { message->clear(); return 0; }
        //     *message+=buffer;
        //     return n;
        // }

        ssize_t Recv(std::string *out) override
        {
            char inbuffer[4096];
            ssize_t n = ::recv(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0);
            if (n > 0)
            {
                inbuffer[n] = 0;
                *out += inbuffer;
            }
            return n;
        }

    private:
        int _sockfd;
        bool _is_server;  // 标记是否为服务器套接字
    };


    class UdpSocket:public Socket
    {
    public:
        UdpSocket() : _sockfd(-1){}
        ~UdpSocket() override { Close(); }

        void CreateListenOrDie(int /*backlog*/ = DEFAULT_BACKLOG) override { /* no-op for UDP */ }

        int Acceptor(InetAddr* cliaddr,int* code) override
        {
            *code = EOPNOTSUPP; 
            return -1;     // UDP 无 accept 语义
        }


        void CreateSocketOrDie() override
        {
            _sockfd = ::socket(AF_INET,SOCK_DGRAM,0);
            if(_sockfd < 0)
            {
                perror("socket");
                exit(2);
            }
        }


        bool Connector(const std::string& ip, uint16_t port) override
        {
            sockaddr_in remote{};
            remote.sin_family = AF_INET;
            remote.sin_port   = htons(port);
            if (::inet_pton(AF_INET, ip.c_str(), &remote.sin_addr) != 1) {
                std::fprintf(stderr, "inet_pton failed for %s\n", ip.c_str());
                std::exit(2);
            }
            if (::connect(_sockfd, (sockaddr*)&remote, sizeof(remote)) < 0) {
                perror("udp connect"); std::exit(2);
            }
            return true;
        }
        

        void Close() override 
        {
            if (_sockfd >= 0) { ::close(_sockfd); _sockfd = -1; }
        }

        int Sockfd() const override
        {
            return _sockfd;
        }

        void CreateBindOrDie(uint16_t port) override
        {
            int opt = 1;
            (void)::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
            sockaddr_in local{};
            local.sin_family      = AF_INET;
            local.sin_port        = htons(port);
            local.sin_addr.s_addr = htonl(INADDR_ANY);
        
            if (::bind(_sockfd, reinterpret_cast<sockaddr*>(&local), sizeof(local)) < 0) {
                perror("bind"); std::exit(2);
            }
        }
        

        ssize_t Send(const std::string& message) override {
            return ::send(_sockfd, message.data(), message.size(), 0);
        }
    
        ssize_t Recv(std::string* message) override {
            char buffer[2048];
            ssize_t n = ::recv(_sockfd, buffer, sizeof(buffer), 0);
            if (n < 0) { perror("recvfrom/recv"); std::exit(2); }
            if (n == 0) { message->clear(); return 0; }
            message->assign(buffer, static_cast<size_t>(n));
            return n;
        }

    private:
        int _sockfd;
    };
}