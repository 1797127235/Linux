#pragma once
#include"Socket.hpp"
#include"Connection.hpp"
#include"Epoller.hpp"
#include "InetAddr.hpp"
#include<unordered_map>
#include<memory>


class Reactor
{
    static const int gnum = 64;
public:
    Reactor() : _epoller(std::make_unique<Epoller>())
    {
        _isrunning = false;
    }
    ~Reactor() = default;

    void EnableConnectionReadWrite(int sockfd,bool readable,bool writable)
    {
        if(!_connections.count(sockfd))
            return;
        
        uint32_t events = ((readable ? EPOLLIN : 0) | (writable ? EPOLLOUT : 0) | (EPOLLET));
        _connections[sockfd]->SetEvents(events);
        
          _epoller->ModEvent(_connections[sockfd]->Sockfd(), _connections[sockfd]->Events());
    }

    //删除连接
    void DelConnection(int sockfd)
    {
        if(!_connections[sockfd]) //防止重复删除
        {
            return ;
        }

        EnableConnectionReadWrite(sockfd,false,false);
        
        //将sockfd移除内核
        _epoller->DelEvent(sockfd);

        //关闭sockfd
        _connections[sockfd]->Close();

        //在Reactor中删除对sockfd的关心
        delete _connections[sockfd];
        _connections.erase(sockfd);
    }

    //添加连接
    void AddConnection(int sockfd,uint32_t events,const InetAddr& addr,int type)
    {
        Connection* conn = new Connection(sockfd);
        conn->SetEvents(events);
        conn->SetAddr(addr);
        conn->SetConnectionType(type);
        conn->SetReactor(this);
        
        if(conn->Type() == ListenConnection)
        {
            conn->RegisterHandler(_OnConnect,nullptr,nullptr);
        }
        else
        {
            conn->RegisterHandler(_OnRecverl,_OnSender,_OnExcepter);
        }

        //将连接写入内核
        if(!_epoller->AddEvent(sockfd,events))
        {
            delete conn;
            return;
        }
        _connections.insert(std::make_pair(sockfd,conn));
    }

    void Dispatcher() //事件派发
    {
        int timeout = -1;
        _isrunning = true;
        while(_isrunning)
        {
            LoopOnce(timeout);
        }
    }
    

    void LoopOnce(int timeout) //一次事件派发
    {
        int n = _epoller->Wait(_events,gnum,timeout);
        
        for(int i = 0; i < n; i++)
        {
            int sockfd = _events[i].data.fd;
            uint32_t events = _events[i].events;
            
            if (events & EPOLLERR)
                events |= (EPOLLIN | EPOLLOUT);
            if (events & EPOLLHUP)
                events |= (EPOLLIN | EPOLLOUT);

            if(events & EPOLLIN)
            {
                if(_connections.count(sockfd) && _connections[sockfd]->_handler_recver)
                {
                    _connections[sockfd]->_handler_recver(_connections[sockfd]);
                }
            }
            if(events & EPOLLOUT)
            {
                if(_connections.count(sockfd) && _connections[sockfd]->_handler_sender)
                {
                    _connections[sockfd]->_handler_sender(_connections[sockfd]);
                }
            }
        }
    }

    void SetOnConnect(handler_t OnConnect)
    {
        _OnConnect = OnConnect;
    }


    void SetOnNormalHandler(handler_t OnRecverl,handler_t OnSender,handler_t OnExcepter)
    {
        _OnRecverl = OnRecverl;
        _OnSender = OnSender;
        _OnExcepter = OnExcepter;
    }

private:
    std::unordered_map<int,Connection*> _connections;
    std::unique_ptr<Epoller> _epoller;

    bool _isrunning = false;
    struct epoll_event _events[1024];

    //Reactor中处理socket的方法集合
    //处理新连接
    handler_t _OnConnect;
    //处理普通socket
    handler_t _OnRecverl;
    handler_t _OnSender;
    handler_t _OnExcepter;
};