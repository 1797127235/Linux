#include"UdpServer.hpp"
#include"Dict.hpp"
#include<memory>

int main(int argc,char* argv[])
{
    EnableScreen();
    if(argc != 2)
    {
        std::cout << "Usage: ./udpserver <IP> <PORT>" << std::endl;
        return 1;
    }
    Dict dict("./data.txt");
    uint16_t port = atoi(argv[1]);
    //创建服务器对象
    std::unique_ptr<UdpServer>  usvr = std::make_unique<UdpServer>(port);
    auto func = std::bind(&Dict::translate,&dict,std::placeholders::_1);
    usvr->SetMessageCallback(func);
    //开启服务器
    usvr->Start();
    return 0;
}