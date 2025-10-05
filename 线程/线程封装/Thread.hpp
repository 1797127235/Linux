#include<pthread.h>
#include<iostream>
#include<string>
#include<functional>
using id = pthread_t;

using func_t = std::function<void()>;
using Task = std::function<void()>;  // 任务类型

class Thread
{
public:
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    Thread() noexcept:
        _tid(0),
        _joinable(false)
    {}

    //移动语句
    Thread(Thread&& other) noexcept:
        _tid(other._tid),
        _joinable(other._joinable)
    {
        other._tid = 0;
        other._joinable = false;
    }

    Thread& operator=(Thread&& other) noexcept
    {
        if(this != &other)
        {
            if(_joinable) //防止资源泄漏
            {
                std::terminate();
            }
            _tid = other._tid;
            _joinable = other._joinable;
            other._tid = 0;
            other._joinable = false;
        }
        return *this;
    }

    template<typename Fn, typename... Args>
    explicit Thread(Fn&& fn, Args&&... args):
        _tid(0),
        _joinable(false)
    {
        // 注意：std::bind 默认“按值拷贝”参数；想按引用传入请用 std::ref(...)
        Task* task = new Task(std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
        int ec  = pthread_create(&_tid,nullptr,&Thread::trampoline,task);
        if(ec != 0)
        {
            delete task;
            throw std::system_error(ec, std::system_category(), "pthread_create failed");
        }
        _joinable = true;
    }
    ~Thread()
    {
        if(_joinable) std::terminate();
    }

    bool joinable() const noexcept
    {
        return _joinable;
    }

    void join()
    {
        if(!_joinable)
        {
            throw std::system_error(EINVAL, std::generic_category(), "not joinable");
        }
        int ec = pthread_join(_tid, nullptr);
        if(ec != 0)
        {
            throw std::system_error(ec, std::generic_category(), "pthread_join");
        }
        _joinable = false;
    }

    void detach()
    {
        if (!_joinable)
            throw std::system_error(EINVAL, std::system_category(), "thread not joinable");
        int ec = ::pthread_detach(_tid);

        if(ec != 0)
        {
            throw std::system_error(ec, std::generic_category(), "pthread_detach");
        }
        _joinable = false;
    }

private:
    static void* trampoline(void* p)
    {
        Task* task = static_cast<Task*>(p);
        try{
            (*task)();
        }catch(...){
            delete task;
            throw;
        }
        delete task;
        return nullptr;
    }

private:
    pthread_t _tid;
    bool _joinable ;
};