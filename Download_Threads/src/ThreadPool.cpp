#include "ThreadPool.h" 

Pool::Pool() : DEFAULT(10)
{
    for (int i = 0; i < DEFAULT; i++) 
    {
        freePool.push_back(std::thread(Func));
    }
}

void Pool::Func() //线程函数,循环等待获取任务
{
    
}

void Pool::addTask(Task task) 
{
    syncQueue.Add(task);
}

void Pool::stopPool() 
{
   
    std::once_flag tmp;
    std::call_once(tmp, [this](){
        running = false;
        // for (auto x = freePool.begin(), end = freePool.end(); x!= end; x++) 
        // {
        //     x->join();
        // }
        for (auto& x : freePool) 
        {
            x.join();
        }
    });
};