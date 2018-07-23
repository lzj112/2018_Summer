#include "Epoll.h"
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <cstring>
using namespace std;

Epoll::Epoll(int fd) 
{
    epoll_Create(fd);
}

void Epoll::epoll_Create(int fd) //初始化　创建epoll句柄
{
    stopEpoll = true;
    listenfd = fd;
    epollFd = epoll_create(1);

    setNonblockFd(fd);
    epoll_Ctl(fd, EPOLL_CTL_ADD);
    assert(epollFd != -1);
}

void Epoll::setNonblockFd(int fd) //设置为非阻塞
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

void Epoll::epoll_Ctl(int fd, int op)   //修改事件合集(添加/删除)
{
    // setNonblockFd(fd);

    epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollFd, op, fd, &ev);
}

void Epoll::Stop() //停止程序
{
    stopEpoll = false;
    taskQueue.stopAll();
    threadPool.stopPool();
}

Epoll::~Epoll() 
{
    close(listenfd);
}

void Epoll::disconnect(int fd, int err) //判断连接断开是否正常
{
    if (err == EINTR || err == EWOULDBLOCK || err == EAGAIN) 
    {
        return ;
    }
    else 
    {
        cout << fd << "与服务器断开连接" << endl;
        epoll_Ctl(fd, EPOLL_CTL_DEL);
    }
}

void Epoll::epoll_Run() 
{
    int ret;
    while (stopEpoll) 
    {
        ret = epoll_wait(epollFd, events, FDNUMBER, 0); //执行一次非阻塞检查
        assert(ret != -1);

        if (ret == 0) //无事件
        {
            continue;
        }
        else 
        {
            epollET(epollFd, events, ret);  //有事件
        }
    }
}

int Epoll::newConnect(int listenfd) //新的连接
{
    sockaddr_in client;
    socklen_t cliLength = sizeof(client);
    int connfd = accept(listenfd, (sockaddr*)&client, &cliLength); //读取新连接
    setNonblockFd(connfd);  //设置为非阻塞
    return connfd;
}

void Epoll::reply(int fd) 
{
    char buffer[] = "查无此文件!";
    send(fd, buffer, sizeof(buffer), 0);
}

void Epoll::assignedTask(int fd) //读取客户端的下载请求并分配任务
{
    Task job;
    Buff buffer;
    int ret = 0;
    while (ret = recv(fd, (void*)&buffer, sizeof(buffer), 0)) //读取客户端发来的请求
    {
        //这里...
    }
        
    if (ret < 0) 
    {
        disconnect(fd, errno);  //判断是否是因为断开连接
    }

    ifstream fin(buffer.from);
    if (!fin.is_open())     //无次文件
    {
        reply(fd);
    }
    struct stat s;
    int size = stat(buffer.from, &s);   //获取文件的大小

    int tmp = buffer.num;   //分成num个任务块
    for (int i = 0; i < tmp; i++) 
    {
        job.Id = i;
        job.pathName = buffer.from;
        job.To = buffer.to;
        job.clientFd = fd;
        job.Size = size;
        job.Location = size * job.Id / buffer.num;
        job.Bytes = size / buffer.num;
        job.num = buffer.num;
        taskQueue.Add(job);
        memset(&job, 0, sizeof(job));
    }
}

void Epoll::epollET(int epollFd, epoll_event* events, int ret) 
{
    for (int i = 0; i < ret; i++) 
    {
        if (events[i].data.fd == listenfd) 
        {
            int connfd = newConnect(listenfd);
            epoll_Ctl(connfd, EPOLL_CTL_ADD);   //将新的连接socketfd添加到合集
        }
        else if (events[i].events & EPOLLIN) 
        {
            assignedTask(events[i].data.fd);
        }
    }
}