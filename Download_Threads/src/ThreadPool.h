#include "TaskSyncQueue.h"
#include <thread>
#include <vector>

class Pool 
{
private:
    std::vector<std::thread> freePool;    //空闲线程池
    std::vector<std::thread> busyPool;    //忙碌线程池
    std::mutex my_Lock;
    TaskQueue syncQueue;
    bool running;            
    const int DEFAULT;
public:
    Pool();
    void Func();
    void stopPool();
    void addTask(Task task);
};