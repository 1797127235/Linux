#include"Server.hpp"
#include"IO.hpp"
#include"http.hpp"

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    Http http;

    IO io(std::bind(&Http::Handle,&http,std::placeholders::_1));
    service_io_t server_io = std::bind(&IO::Excute,&io,std::placeholders::_1,std::placeholders::_2);
    std::unique_ptr<Server> tsvr = std::make_unique<Server>(atoi(argv[1]),server_io);
    tsvr->loop();
    return 0;
}