#include "TaskSyncQueue.h" 


TaskQueue::TaskQueue() : stopQueue(false), Empty(true) 
{

}

int TaskQueue::get_Size() 
{
    return Queue.empty() ? 0 : Queue.size();
}

void TaskQueue::Add(Task task) 
{
    my_Lock.lock();
    if (stopQueue) 
    {
        return ;
    }
    Queue.push_back(task); //添加任务
    Empty = false;         //任务队列不为空
    cv_Any.notify_one();   //唤醒一个因为队列空而阻塞的线程
    my_Lock.unlock();
}

void TaskQueue::Take(Task& task) 
{
    my_Lock.lock();
    while (Empty)  //等待任务队列不为空 
    {
        cv_Any.wait(my_Lock);
    }
    if (stopQueue) 
    {
        return ;
    }
    task = Queue.front();   //取走一个任务
    Queue.pop_back();       //删除那个任务
    if (get_Size() == 0)    //如果队列为空
    {
        Empty = true;
    }
    my_Lock.unlock();
}

void TaskQueue::stopAll() 
{
    std::once_flag tmp;
    std::call_once(tmp, [this](){
        stopQueue = true;
        Empty = false;
        cv_Any.notify_all();    //唤醒所有等待线程
    });
}