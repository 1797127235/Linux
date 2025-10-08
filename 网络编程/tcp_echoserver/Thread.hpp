#pragma once
#include <string>
#include <functional>
#include <pthread.h>

namespace ThreadMoudle
{
    // 线程要执行的方法，后面我们随时调整
    // typedef void (*func_t)(ThreadData *td); // 函数指针类型

    // typedef std::function<void()> func_t;
    using func_t = std::function<void(const std::string&)>;

    class Thread
    {
    public:
        void Excute()
        {
            _isrunning = true;
            _func(_name);
            _isrunning = false;
        }
    public:
        Thread(const std::string &name, func_t func):_name(name), _func(func)
        {
        }
        static void *ThreadRoutine(void *args) // 新线程都会执行该方法！
        {
            Thread *self = static_cast<Thread*>(args); // 获得了当前对象
            self->Excute();
            return nullptr;
        }
        bool Start()
        {
            int n = ::pthread_create(&_tid, nullptr, ThreadRoutine, this);
            if(n != 0) return false;
            return true;
        }
        std::string Status()
        {
            if(_isrunning) return "running";
            else return "sleep";
        }
        void Stop()
        {
            if(_isrunning)
            {
                ::pthread_cancel(_tid);
                _isrunning = false;
            }
        }
        void Join()
        {
            ::pthread_join(_tid, nullptr);
        }
        std::string Name()
        {
            return _name;
        }
        ~Thread()
        {
        }

    private:
        std::string _name;
        pthread_t _tid;
        bool _isrunning;
        func_t _func; // 线程要执行的回调函数
    };
} // namespace ThreadModle
