#pragma once
#include<iostream>
#include<functional>
#include<string>
#include"Socket.hpp"
#include"InetAddr.hpp"
#include"http.hpp"
using process_t = std::function<std::string(std::string)>;
using namespace socket_ns;
class IO
{
public:
    IO(process_t process):_process(process){}

    void Excute(SockPtr clientsock,InetAddr& client)
    {
        std::string msg; 
        while(true)
        {
            int n = clientsock->Recv(&msg);
            if(n == 0)
            {
                break;
            }

            std::string response = _process(msg);
            clientsock->Send(response);
        }
    }

    ~IO()
    {}
private:
    process_t _process;
};