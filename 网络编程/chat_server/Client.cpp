#include <cstdint>
#include<iostream>
#include<string>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<thread>

using std::thread;

int Init()
{
    int sockfd = ::socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        return 1;
    }

    return sockfd;
}

void RecvMessage(int sockfd)
{
    while (true)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        char buffer[1024];
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cerr << buffer << std::endl;
        }
        else
        {
            std::cerr << "recvfrom error" << std::endl;
            break;
        }
    }
}

void SendMessage(int sockfd, std::string serverip, uint16_t serverport, const std::string &name)
{
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());

    std::string cli_profix = name + "# "; // sender-thread# 你好
    std::string register_msg = "REGISTER " + name;
    int ret = sendto(sockfd, register_msg.c_str(), register_msg.size(), 0, (struct sockaddr *)&server, sizeof(server));
    if (ret <= 0)
    {
        std::cerr << "register message send error" << std::endl;
        return;
    }
    while (true)
    {
        std::string line;
        std::cout << cli_profix;
        std::getline(std::cin, line);
        int n = sendto(sockfd, line.c_str(), line.size(), 0, (struct sockaddr *)&server, sizeof(server));
        if (n <= 0)
            break;
    }
}


int main(int argc,char* argv[])
{
    if(argc < 3)
    {
        std::cout << "Usage: ./udpclient <IP> <PORT> [NAME]" << std::endl;
        return 1;
    }

    int sockfd = Init();

    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);
    std::string name;
    if (argc >= 4)
    {
        name = argv[3];
    }
    else
    {
        name = "user-" + std::to_string(getpid());
    }


    thread t1(RecvMessage,sockfd);

    thread t2(SendMessage,sockfd,ip,port,name);


    t1.join();
    t2.join();


    


    ::close(sockfd);
    return 0;
}
