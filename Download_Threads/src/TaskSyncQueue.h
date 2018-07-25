#ifndef TASKSYNCQUEUE_H
#define TASKSYNCQUEUE_H

#include <condition_variable>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include "Task.h"

class TaskQueue 
{
private:
    std::vector<Task> Queue;                //任务队列
    std::mutex my_Lock;                     //互斥锁
    std::condition_variable_any cv_Any;     //条件变量
    bool stopQueue;                         //是否停止
    bool Empty;                             //任务队列是否为空
public:
    TaskQueue();
    int get_Size();                 //计算任务队列内的任务数
    void Add(Task task);            //添加任务
    void Take(Task& task);          //取出任务
    void stopAll();                 //停止 唤醒所有等待线程

};

#endif