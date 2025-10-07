#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string>
#include<sys/socket.h>


class InetAddr
{
public:
    InetAddr(struct sockaddr_in addr):
        _addr(addr)
    {
        _ip = inet_ntoa(_addr.sin_addr);
        _port = ntohs(_addr.sin_port);
    }

    std::string Ip() const
    {
        return _ip;
    }


    uint16_t Port() const
    {
        return _port;
    }

private:
    struct sockaddr_in _addr;
    std::string _ip;
    uint16_t _port;
};