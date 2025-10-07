#include "Route.hpp"
#include"UdpServer.hpp"
#include"Dict.hpp"

#include <functional>
#include<memory>

int main(int argc,char* argv[])
{
    EnableScreen();
    if(argc != 2)
    {
        std::cout << "Usage: ./udpserver <IP> <PORT>" << std::endl;
        return 1;
    }
    uint16_t port = atoi(argv[1]);
    //创建服务器对象
    std::unique_ptr<UdpServer>  usvr = std::make_unique<UdpServer>(port);
    Route r;
    task t = bind(&Route::Forward,&r,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
    usvr->SetMessageCallback(t);

    //开启服务器
    usvr->Start();
    return 0;
}