#pragma once
#include<vector>
#include<iostream>
#include<functional>
#include<ctime>
#include<cstdlib>
#include<unistd.h>
#include<sys/types.h>


const int TASK_NUM = 3;

using task_t = std::function<void()>;

std::vector<task_t> tasks(TASK_NUM);

void Print()
{
    std::cout << "i am a print task" << std::endl;
}

void DownLoad()
{
    std::cout << "i am a download task" << std::endl;
}

void Flush()
{
    std::cout << "i am a flush task" << std::endl;
}

void LoadTask()
{
    srand(time(nullptr)) ^ getpid();
    tasks[0] = Print;
    tasks[1] = DownLoad;
    tasks[2] = Flush;
}

void ExcuteTask(int idx)
{
    if(idx < 0 || idx >= TASK_NUM)
    {
        return;
    }
    tasks[idx]();
}

int SlectTaks()
{
    return rand() % TASK_NUM;
}








