#pragma once
#include <iostream>
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
            std::cout << "Decoded package: " << package << std::endl;
            // 我们能保证我们读到的是一个完整的报文吗?能！！
            auto req = Factory::BuildRequest();

            // 3. 反序列化
            req->Deserialize(package);
            std::cout << "Request: x=" << req->_x << " y=" << req->_y << " op=" << req->_op << std::endl;
            
            // 4. 业务处理
            auto resp = Calc::Process(*req);
            std::cout << "Response: result=" << resp->_result << " code=" << resp->_code << std::endl;

            // 5. 序列化应答
            std::string respjson;
            resp->Serialize(&respjson);

            // 6. 添加len长度报头
            respjson = Encode(respjson);
            std::cout << "Encoded response: " << respjson << std::endl;

            // 7. 发回
            conn->AppendOutbuffer(respjson);
            std::cout << "Appended to outbuffer, size: " << conn->Outbuffer().size() << std::endl;
        }

        // 我们已经至少处理了一个请求，同时至少会有一个应答
        // if(!conn->OutBuffer().empty()) 
        // conn->_handler_sender(conn); // 方法1：直接发送数据
        if (!conn->Outbuffer().empty()) {
            std::cout << "Enabling write events for sockfd " << conn->Sockfd() << std::endl;
            conn->_R->EnableConnectionReadWrite(conn->Sockfd(), true, true); // 方法2：我只要进行对写事件的关心即可
        }
    }

private:
    Calc cal;
};
