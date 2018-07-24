#include "TaskSyncQueue.h"
#include <thread>
#include <vector>

class Pool 
{
private:
    std::vector<std::thread> freePool;    //空闲线程池
    std::vector<std::thread> busyPool;    //忙碌线程池
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