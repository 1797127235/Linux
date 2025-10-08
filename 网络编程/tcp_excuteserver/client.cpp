#include <iostream>
#include<string>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cstring>

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        std::cout << "Usage: ./udpclient <IP> <PORT> [NAME]" << std::endl;
        return 1;
    }
    char* ip = argv[1];
    uint16_t port = atoi(argv[2]);

    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        return 2;
    }
    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    //server.sin_addr.s_addr = inet_addr(ip);
    inet_pton(AF_INET,ip,&server.sin_addr);

    if(::connect(sockfd,(struct sockaddr*)&server,sizeof(server)) < 0)
    {
        std::cerr << "connect error" << std::endl;
        return 2;
    }

    while(true)
    {
        std::cout << "please enter" << std::endl;
        std::string line;
        while(std::getline(std::cin,line))
        {
            std::string cmd = line + "\n"; 
            int n = ::write(sockfd,cmd.c_str(),cmd.size());
            if(n <= 0)
            {
                std::cerr << "write error" << std::endl;
                break;
            }



            char buffer[1024];
            int m = ::read(sockfd,buffer,sizeof(buffer) - 1);
            if(m <= 0)
            {
                std::cerr << "read error" << std::endl;
                break;
            }
            buffer[m] = 0;

            std::cout << buffer << std::endl;
        }
    }
    
    return 0;
}