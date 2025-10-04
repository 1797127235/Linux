#ifndef __SHM_HPP__
#define __SHM_HPP__

#include <iostream>
#include <string>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

const int gCreater = 1;
const int gUser = 2;
const std::string gpathname = "/home/whb/code/111/code/lesson22/4.shm";
const int gproj_id = 0x66;
const int gShmSize = 4097; // 4096*n

class Shm
{
private:
    key_t GetCommKey()
    {
        key_t k = ftok(_pathname.c_str(), _proj_id);
        if (k < 0)
        {
            perror("ftok");
        }
        return k;
    }
    int GetShmHelper(key_t key, int size, int flag)
    {
        int shmid = shmget(key, size, flag);
        if (shmid < 0)
        {
            perror("shmget");
        }

        return shmid;
    }
    std::string RoleToString(int who)
    {
        if (who == gCreater)
            return "Creater";
        else if (who == gUser)
            return "gUser";
        else
            return "None";
    }
    void *AttachShm()
    {
        if (_addrshm != nullptr)
            DetachShm(_addrshm);
        void *shmaddr = shmat(_shmid, nullptr, 0);
        if (shmaddr == nullptr)
        {
            perror("shmat");
        }
        std::cout << "who: " << RoleToString(_who) << " attach shm..." << std::endl;
        return shmaddr;
    }
    void DetachShm(void *shmaddr)
    {
        if (shmaddr == nullptr)
            return;
        shmdt(shmaddr);
        std::cout << "who: " << RoleToString(_who) << " detach shm..." << std::endl;
    }

public:
    Shm(const std::string &pathname, int proj_id, int who)
        : _pathname(pathname), _proj_id(proj_id), _who(who), _addrshm(nullptr)
    {
        _key = GetCommKey();
        if (_who == gCreater)
            GetShmUseCreate();
        else if (_who == gUser)
            GetShmForUse();
        _addrshm = AttachShm();

        std::cout << "shmid: " << _shmid << std::endl;
        std::cout << "_key: " << ToHex(_key) << std::endl;
    }
    ~Shm()
    {
        if (_who == gCreater)
        {
            int res = shmctl(_shmid, IPC_RMID, nullptr);
        }
        std::cout << "shm remove done..." << std::endl;
    }

    std::string ToHex(key_t key)
    {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "0x%x", key);
        return buffer;
    }
    bool GetShmUseCreate()
    {
        if (_who == gCreater)
        {
            _shmid = GetShmHelper(_key, gShmSize, IPC_CREAT | IPC_EXCL | 0666);
            if (_shmid >= 0)
                return true;
            std::cout << "shm create done..." << std::endl;
        }
        return false;
    }
    bool GetShmForUse()
    {
        if (_who == gUser)
        {
            _shmid = GetShmHelper(_key, gShmSize, IPC_CREAT | 0666);
            if (_shmid >= 0)
                return true;
            std::cout << "shm get done..." << std::endl;
        }
        return false;
    }
    void Zero()
    {
        if(_addrshm)
        {
            memset(_addrshm, 0, gShmSize);
        }
    }

    void *Addr()
    {
        return _addrshm;
    }

    void DebugShm()
    {
        struct shmid_ds ds;
        int n = shmctl(_shmid, IPC_STAT, &ds);
        if(n < 0) return;
        std::cout << "ds.shm_perm.__key : " << ToHex(ds.shm_perm.__key)  << std::endl;
        std::cout << "ds.shm_nattch: " << ds.shm_nattch << std::endl;
    }

private:
    key_t _key;
    int _shmid;

    std::string _pathname;
    int _proj_id;

    int _who;
    void *_addrshm;
};

#endif