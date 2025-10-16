#include"SelectServer.hpp"
#include<memory>

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    uint16_t port = std::atoi(argv[1]);

    std::unique_ptr<SelectServer> svr = std::make_unique<SelectServer>(port);

    svr->Init();
    svr->loop();
    return 0;
    
}