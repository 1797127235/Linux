#include<iostream>
#include<functional>
#include"Socket.hpp"
#include"InetAddr.hpp"
#include"Calc.hpp"
#include"Protocol.hpp"
using process_t = std::function<std::shared_ptr<Response>(Request)>;


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
            std::string json;
            if(!Decode(msg,json))
            {
                continue;   
            }

            //反序列化
            Request req;
            req.Deserialize(json);
            

            //计算
            std::shared_ptr<Response> resp = _process(req);

            std::string response_json;
            resp->Serialize(&response_json);

            //发送
            std::string response_msg = Encode(response_json);
            clientsock->Send(response_msg);
        }
    }

    ~IO()
    {}
private:
    process_t _process;
};