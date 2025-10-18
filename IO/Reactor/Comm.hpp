#pragma once

#include<iostream>
#include<unistd.h>
#include<fcntl.h>

enum
{
    SOCKET_ERROT = 1,
    BIND_ERROR,
    LISTEN_ERROR,
    EPOLL_CREATE_ERROR
};

void SetNonBlock(int fd)
{
    int fl = ::fcntl(fd, F_GETFL);
    if (fl < 0)
    {
        std::cout << "fcntl error" << std::endl;
        return;
    }
    ::fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}