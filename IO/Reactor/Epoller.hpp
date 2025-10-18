#pragma once

#include<sys/epoll.h>
#include<cerrno>
#include<string>

class Multiplex
{
public:
    virtual bool AddEvent(int fd, uint32_t events) = 0;
    virtual int Wait(struct epoll_event revs[], int num, int timeout) = 0;
    virtual bool ModEvent(int fd, uint32_t events) = 0;
    virtual bool DelEvent(int fd) = 0;
};


class Epoller : public Multiplex
{
public:
    Epoller()
    {
        _epfd = ::epoll_create1(0);
        if(_epfd == -1)
            throw std::runtime_error("epoll_create1 error");
    }
    ~Epoller()
    {
        if(_epfd >= 0)
            ::close(_epfd);
    }

    Epoller(const Epoller&) = delete;
    Epoller& operator=(const Epoller&) = delete;

    Epoller(Epoller&& other) noexcept : _epfd(other._epfd) { other._epfd = -1; }
    Epoller& operator=(Epoller&& other) noexcept {
        if (this != &other) {
            if (_epfd != -1) ::close(_epfd);
            _epfd = other._epfd;
            other._epfd = -1;
        }
        return *this;
    }

    bool AddEvent(int fd, uint32_t events) override
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = fd;
        int ret = ::epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&ev);
        if(ret == -1)
            return false;
        return true;
    }

    int Wait(struct epoll_event revs[], int num, int timeout) override
    {
        return ::epoll_wait(_epfd,revs,num,timeout);
    }

    bool ModEvent(int fd, uint32_t events) override
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = fd;
        int ret = ::epoll_ctl(_epfd,EPOLL_CTL_MOD,fd,&ev);
        if(ret == -1)
            return false;
        return true;
    }

    bool DelEvent(int fd) override
    {
        int ret = ::epoll_ctl(_epfd,EPOLL_CTL_DEL,fd,nullptr);
        if(ret == -1)
            return false;
        return true;
    }
    

private:
    int _epfd;
};