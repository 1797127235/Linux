#pragma once

#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <queue>
#include <functional>
#include "Thread.hpp"
#include "Log.hpp"
#include "Lockguard.hpp"

using namespace ThreadMoudle;
using namespace log_ns;

static const int gdefaultnum = 5;

void test()
{
    while (true)
    {
        std::cout << "hello world" << std::endl;
        sleep(1);
    }
}

template <typename T>
class ThreadPool
{
private:
    void LockQueue()
    {
        pthread_mutex_lock(&_mutex);
    }
    void UnlockQueue()
    {
        pthread_mutex_unlock(&_mutex);
    }
    void Wakeup()
    {
        pthread_cond_signal(&_cond);
    }
    void WakeupAll()
    {
        pthread_cond_broadcast(&_cond);
    }
    void Sleep()
    {
        pthread_cond_wait(&_cond, &_mutex);
    }
    bool IsEmpty()
    {
        return _task_queue.empty();
    }
    void HandlerTask(const std::string &name) // this
    {
        while (true)
        {
            // 取任务
            LockQueue();
            while (IsEmpty() && _isrunning)
            {
                _sleep_thread_num++;
                Sleep();
                _sleep_thread_num--;
            }
            // 判定一种情况
            if (IsEmpty() && !_isrunning)
            {
                UnlockQueue();
                break;
            }

            // 有任务
            T t = _task_queue.front();
            _task_queue.pop();
            UnlockQueue();

            // 处理任务
            t(); // 处理任务，此处不用/不能在临界区中处理
            // std::cout << name << ": " << t.result() << std::endl;
            // LOG(DEBUG, "hander task done, task is : %s\n", t.result().c_str());
        }
    }
    void Init()
    {
        func_t func = std::bind(&ThreadPool::HandlerTask, this, std::placeholders::_1);
        for (int i = 0; i < _thread_num; i++)
        {
            std::string threadname = "thread-" + std::to_string(i + 1);
            _threads.emplace_back(threadname, func);
        }
    }
    void Start()
    {
        _isrunning = true;
        for (auto &thread : _threads)
        {
            thread.Start();
        }
    }
    ThreadPool(int thread_num = gdefaultnum)
        : _thread_num(thread_num), _isrunning(false), _sleep_thread_num(0)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }
    ThreadPool(const ThreadPool<T> &) = delete;
    void operator=(const ThreadPool<T> &) = delete;

public:
    void Stop()
    {
        LockQueue();
        _isrunning = false;
        WakeupAll();
        UnlockQueue();
    }

    // 如果是多线程获取单例呢？
    static ThreadPool<T> *GetInstance()
    {
        if (_tp == nullptr)
        {
            LockGuard lockguard(&_sig_mutex);
            if (_tp == nullptr)
            {
                // thread-1 thread-2 thread-3....
                _tp = new ThreadPool<T>();
                _tp->Init();
                _tp->Start();
            }
            else
            {
                // LOG(INFO, "get threadpool\n");
            }
        }
        return _tp;
    }

    void Equeue(const T &in)
    {
        LockQueue();
        if (_isrunning)
        {
            _task_queue.push(in);
            if (_sleep_thread_num > 0)
                Wakeup();
        }
        UnlockQueue();
    }
    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }

private:
    int _thread_num;
    std::vector<Thread> _threads;
    std::queue<T> _task_queue;
    bool _isrunning;

    int _sleep_thread_num;

    pthread_mutex_t _mutex;
    pthread_cond_t _cond;

    // 单例模式
    // volatile static ThreadPool<T> *_tp;
    static ThreadPool<T> *_tp;
    static pthread_mutex_t _sig_mutex;
};

template <typename T>
ThreadPool<T> *ThreadPool<T>::_tp = nullptr;
template <typename T>
pthread_mutex_t ThreadPool<T>::_sig_mutex = PTHREAD_MUTEX_INITIALIZER;
