#include"EpollServer.hpp"
#include<memory>


int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    uint16_t port = std::atoi(argv[1]);
    std::unique_ptr<EpollServer> server = std::make_unique<EpollServer>(port);
    server->loop();
    return 0;
}
