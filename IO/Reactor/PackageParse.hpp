#pragma once
#include <functional>
#include "InetAddr.hpp"
#include "Protocol.hpp"
#include "Connection.hpp"
#include "Calc.hpp"

class PackageParse
{
public:
    void Excute(Connection *conn)
    {
        while (true)
        {
            // 我们能保证我们读到的是一个完整的报文吗？不能！
            // 2. 报文解析，提取报头和有效载荷
           std::string package;
           if(!Decode(conn->Inbuffer(),package))
                break;

            // 我们能保证我们读到的是一个完整的报文吗?能！！
            auto req = Factory::BuildRequest();

            // 3. 反序列化
            req->Deserialize(package);
            
            // 4. 业务处理
            auto resp = Calc::Process(*req);

            // 5. 序列化应答
            std::string respjson;
            resp->Serialize(&respjson);

            // 6. 添加len长度报头
            respjson = Encode(respjson);

            // 7. 发回
            conn->AppendOutbuffer(respjson);
        }


        //设置对写事件关心
        if (!conn->Outbuffer().empty()) {
            conn->_R->EnableConnectionReadWrite(conn->Sockfd(), true, true); // 方法2：我只要进行对写事件的关心即可
        }
    }

private:
    Calc cal;
};
