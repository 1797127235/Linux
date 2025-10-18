#pragma once

#include"Connection.hpp"
#include<iostream>
#include<errno.h>
#include<sys/types.h>

const size_t buffersize = 512;
class HandlerConnection
{
public:
    HandlerConnection(handler_t process):
    _process(process)
    {
    }
    void HandleRecver(Connection* conn)
    {
        //非阻塞读取
        errno = 0;
        while(true)
        {
            char buffer[buffersize];
            int n = ::recv(conn->Sockfd(),buffer,buffersize - 1,0);
            if(n > 0)
            {
                buffer[n] = 0;
                conn->AppendInbuffer(buffer);
            }
            else
            {
                if(errno == EWOULDBLOCK)
                {
                    break;
                }
                else if(errno == EINTR)
                {
                    continue;
                }
                else
                {
                    conn->_handler_excepter(conn);
                    return ;
                }
            }
        }
        _process(conn);
    }

    void HandleSender(Connection* conn)
    {
        //一次性发完
        errno = 0;
        while(true)
        {
            int n = ::send(conn->Sockfd(),conn->Outbuffer().c_str(),conn->Outbuffer().size(),0);
            if(n > 0)
            {
                conn->DiscardOutbuffer(n);
                if(conn->Outbuffer().empty())
                {
                    break;
                }
            }
            else if(n == 0) //发送完了
            {
                break;
            }
            if(n < 0)
            {
                if(errno == EWOULDBLOCK)
                {
                    //发送条件不满足
                    break;
                }
                else if(errno == EINTR)
                {
                    continue;
                }
                else
                {
                    conn->_handler_excepter(conn);
                    return ;
                }
            }
        }

        //发现不为空时继续写
        if(!conn->Outbuffer().empty())
        {
            conn->_R->EnableConnectionReadWrite(conn->Sockfd(),true,true);
        }
        else
        {
            conn->_R->EnableConnectionReadWrite(conn->Sockfd(),true,false);
        }

    }

    void HandleExcepter(Connection* conn)
    {
        conn->_R->DelConnection(conn->Sockfd());
    }
private:
    handler_t _process;
};