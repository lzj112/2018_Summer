#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
// #include <vector>
#include "Task.h"
#include "TaskSyncQueue.h"
#include "ThreadPool.h"
const int FDNUMBER = 1024;

class Epoll 
{
private:
    int epollFd;            //epoll句柄
    // std::vector<epoll_event*> events;
    epoll_event events[FDNUMBER];  //fd合集
    int listenfd;             //监听socketfd
    bool stopEpoll;         //停止标志
    Pool threadPool;        //线程池
public:
    Epoll(int fd);
    ~Epoll();
    void epoll_Create(int fd);
    void epoll_Ctl(int fd, int op);
    void epoll_Run();
    void Stop();
    void epollET(int epollFd, epoll_event* EVENTS, int ret);
    int disconnect(int fd, int err);
    void setNonblockFd(int fd);   
    void assignedTask(int listenfd);   
    void reply(int fd);
    int newConnect(int listenfd);
    
};

#endif