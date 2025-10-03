#include<iostream>
#include<string>
#include<sys/types.h>
#include<cerrno>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

const std::string PATH = "./myfifo";
const int CREATER = 1;
const int USER = 2;
const int BASESIZE = 1024;


class Pipe
{
public:
    Pipe(std::string  path = PATH,int who = CREATER):_path(path),_id(who)
    {
        if(_id == CREATER)
        {
            CreateNamePipe(_path);
        }
    }

    ~Pipe()
    {
        if(_id == CREATER)
        {
            RemoveNamePipe(_path);
        }
        if(_fd != -1) close(_fd);
    }

    bool OpenForRead()
    {
        return OpenPipe(O_RDONLY);
    }

    bool OpenForWrite()
    {
        return OpenPipe(O_WRONLY);
    }

    int GetFd()
    {
        return _fd;
    }

    int ReadPipe(std::string* out)
    {
        char buf[BASESIZE] = {0};
        int n = read(_fd,buf,sizeof(buf)-1);
        if(n > 0)
        {
            buf[n] = 0;
            *out = buf;
        }
        return n;
    }

    int WritePipe(const std::string & in)
    {
        return write(_fd,in.c_str(),in.size());
    }



private:
    int CreateNamePipe(std::string & path)
    {
        int ret = mkfifo(path.c_str(),0666);
        if(ret  != 0)
        {
            perror("mkfifo");
            return -1;
        }
        return ret;
    }

    bool OpenPipe(int mode)
    {
        _fd = open(_path.c_str(),mode);
        if(_fd < 0)
        {
            perror("open");
            return false;
        }
        return true;
    }


    void RemoveNamePipe(std::string & path)
    {
        unlink(path.c_str());
    }

private:
    std::string _path = PATH;
    int _id = -1;
    int _fd = -1;
};