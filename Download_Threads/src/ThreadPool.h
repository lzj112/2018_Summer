#ifndef THREADPOLL_h
#define THREADPOLL_h

#include "TaskSyncQueue.h"
#include <thread>
#include <vector>

class Pool 
{
private:
    std::vector<std::thread> threads;    //线程池
    //尝试:智能指针存放线程指针来管理

    int poolInFree;       //空闲的线程量
    std::mutex my_Lock;
    TaskQueue syncQueue;                  //任务同步队列
    bool running;            
    const int DEFAULT;
public:
    Pool();
    void performTask();
    void stopPool();
    void addTask(Task task);
    void readFile(Task job);
};

#endif