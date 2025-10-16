#include"Socket.hpp"
#include<thread>
#include<functional>
using namespace socket_ns;

using service_io_t = std::function<void(SockPtr clientsock,InetAddr& client)>;
class Server
{
public:
    Server(uint16_t port,service_io_t service):
    _listensock(new TcpSocket),
    _isrunning(false),
    _service(service)
    {
        _listensock->BuildListenSocket(port);
    }

    void loop()
    {
        _isrunning = true;
        while(_isrunning)
        {
            InetAddr client;
            SockPtr clientsock = _listensock->Acceptor(&client);
            if(clientsock)
            {
                std::thread t(_service,clientsock,std::ref(client));
                t.detach();
            }
        }
    }

    void Stop()
    {
        if(_isrunning)
        {
            _isrunning = false;
            _listensock->Close();
        }
    }
private:
    SockPtr _listensock;
    bool _isrunning;
    service_io_t _service;
};