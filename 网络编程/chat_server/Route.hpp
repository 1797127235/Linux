#pragma once
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include<vector>
#include<algorithm>
#include<string>
#include"InetAddr.hpp"
#include "Lockguard.hpp"
#include"ThreadPool.hpp"
#include"Log.hpp"

using task_t = std::function<void()>;
class Route
{
public:
    Route()
    {
        pthread_mutex_init(&_mtx,nullptr);
    }
    bool CheckOnlineUser(InetAddr &who)
    {
        LockGuard lockguard(&_mtx);
        if (std::find(_onlineuser.begin(), _onlineuser.end(), who) == _onlineuser.end())
        {
            LOG(INFO, "%s register success\n", who.AddrStr().c_str());
            _onlineuser.push_back(who);
            return true;
        }
        return false;
    }

    void Offline(InetAddr &who)
    {
        LockGuard lockguard(&_mtx);
        auto iter = _onlineuser.begin();
        for (; iter != _onlineuser.end(); iter++)
        {
            if (*iter == who)
            {
                LOG(DEBUG, "%s is offline\n", who.AddrStr().c_str());
                _onlineuser.erase(iter);
                break;
            }
        }
    }

    void ForwardForEach(int sockfd,const std::string& msg,InetAddr& who)
    {
        LockGuard lock(&_mtx);

        if (msg.rfind("REGISTER ", 0) == 0)
        {
            LOG(DEBUG, "Ignore register broadcast from %s\n", who.AddrStr().c_str());
            return;
        }

        std::string send_message = "[" + who.AddrStr() + "]# " + msg;
        for (auto &user : _onlineuser)
        {
            if(user == who) continue;
            
            struct sockaddr_in peer = user.addr();
            LOG(DEBUG, "Forward message to %s, message is %s\n", user.AddrStr().c_str(), send_message.c_str());
            ::sendto(sockfd, send_message.c_str(), send_message.size(), 0, (struct sockaddr *)&peer, sizeof(peer));
        }
    }
    

    void Forward(int sockfd,const std::string& msg,InetAddr& who)
    {
        CheckOnlineUser(who);
        if(msg == "QUIT" || msg == "quit")
        {
            Offline(who);
        }


        task_t task = std::bind(&Route::ForwardForEach,this,sockfd,msg,who);

        ThreadPool<task_t>::GetInstance()->Equeue(task);
    }

    ~Route()
    {
        pthread_mutex_destroy(&_mtx);
    }
private:
    std::vector<InetAddr> _onlineuser; //在线用户列表
    pthread_mutex_t _mtx;
};
