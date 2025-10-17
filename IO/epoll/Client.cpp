#include"Socket.hpp"
#include <iostream>
#include <sstream>
using namespace socket_ns;

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <ip> <port>" << std::endl;
        return 1;
    }
    std::string ip = argv[1];
    uint16_t port = std::atoi(argv[2]);
    std::unique_ptr<TcpSocket> client = std::make_unique<TcpSocket>();
    if(!client->BuildClientSocket(ip,port))
    {
        std::cerr << "Connect failed" << std::endl;
        return 1;
    }

    while(true)
    {
        std::string message;
        std::cout << "Enter message: ";
        std::cin >> message;
        client->Send(message);
        std::string response;
        std::string part;
        ssize_t n = client->Recv(&part);
        if(n <= 0) break;
        response += part;

        std::cout << "Response: " << response << std::endl;
    }
    return 0;
}
