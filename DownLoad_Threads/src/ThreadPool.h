#ifndef THREADPOLL_h
#define THREADPOLL_h

#include "TaskSyncQueue.h"
#include <thread>
#include <vector>

const int MAX_NUMBER = 10;

class Pool 
{
private:
    std::vector<std::thread> threads;    //线程池
    //尝试:智能指针存放线程指针来管理

    int poolInFree;       //空闲的线程量 还没实现
    std::mutex my_Lock;
    TaskQueue syncQueue;                  //任务同步队列
    bool running;            
    // const int DEFAULT;
public:
    Pool();
    void performTask();
    void stopPool();
    void addTask(DownloadMsg task);
    void judge(DownloadMsg job);   //判断是否是断线续传
    void incThreads();
    void sendFromBreak(DownloadMsg job);       //断点续传发送文件
    void sendNoBreak(DownloadMsg job);    //不是断点续传
    int my_pread(int fd, int location, DownloadMsg& job);
};

#endif