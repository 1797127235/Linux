#include"UdpServer.hpp"
#include<memory>

int main(int argc,char* argv[])
{
    EnableScreen();
    if(argc != 3)
    {
        std::cout << "Usage: ./udpserver <IP> <PORT>" << std::endl;
        return 1;
    }
    char * ip = argv[1];
    uint16_t port = atoi(argv[2]);
    //创建服务器对象
    std::unique_ptr<UdpServer>  usvr = std::make_unique<UdpServer>(ip,port);

    //开启服务器
    usvr->Start();
    return 0;
}