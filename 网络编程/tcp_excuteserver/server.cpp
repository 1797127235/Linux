#include "server.hpp"


int main(int argc,char* argv[])
{
    if(argc < 2)
    {
        LOG(ERROR,"Usage: %s <port>",argv[0]);
        return 1;
    }
    uint16_t port = atoi(argv[1]);
    std::unique_ptr<TcpServer> server = std::make_unique<TcpServer>(port);

    Command cmd;
    server->SetTask(std::bind(&Command::Excute,&cmd,std::placeholders::_1,std::placeholders::_2));
    server->Start();
    return 0;
}