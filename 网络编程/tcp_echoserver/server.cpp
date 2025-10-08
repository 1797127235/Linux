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
    server->Start();
    return 0;
}