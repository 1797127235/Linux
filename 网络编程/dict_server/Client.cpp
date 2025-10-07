#include<iostream>
#include<string>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>


int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        std::cout << "Usage: ./udpclient <IP> <PORT>" << std::endl;
        return 1;
    }
    char* ip = argv[1];
    uint16_t port = atoi(argv[2]);
    //创建套接字
    int sockfd = ::socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        return 1;
    }
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = inet_addr(ip);
    socklen_t len = sizeof(serv);

    //与服务器通信
    std::string message;
    while(true)  
    {
        std::cout << "please enter# " << std::endl;
        std::getline(std::cin,message);
        if(message == "quit")
            break;
        ssize_t n = sendto(sockfd,message.c_str(),message.size(),0,(struct sockaddr*)&serv,len);
        if(n < 0)
        {
            std::cerr << "sendto error" << std::endl;
            continue;
        }
        char buffer[1024];
        struct sockaddr_in temp;
        memset(&temp,0,sizeof(temp));
        socklen_t len = sizeof(temp);
        n = recvfrom(sockfd,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&temp,&len);
        if(n < 0){
            std::cerr << "recvfrom error" << std::endl;
            continue;
        }
        
        buffer[n] = 0;
        std::cout << "server# " << buffer << std::endl;
    }

    ::close(sockfd);
    return 0;
}