#include "Epoll.h"
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
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
    threadPool.stopPool();
}

Epoll::~Epoll() 
{
    close(listenfd);
}

int Epoll::disconnect(int fd, int err) //判断连接断开是否正常
{
    if (err == EINTR || err == EWOULDBLOCK || err == EAGAIN) 
    {
        return 0;
    }
    else 
    {
        cout << fd << "与客户端断开连接" << endl;
        return 1;
    }
}

void Epoll::epoll_Run() 
{
    int ret;
    signal(SIGINT, SIG_IGN);    //忽略软中断
    signal(SIGPIPE, SIG_IGN);   //忽略sigpipe 
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
            cout << "有新的请求" << endl;
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

void Epoll::assignedTask(int fd) //读取客户端的下载请求并分配任务
{
    Task job;
    int ret = 0;
    while (1)
    {
        ret = recv(fd, (void*)&job, sizeof(job), 0);
        if (ret <= 0) 
        {
            break;
        }
        cout << "接受请求ing...." << endl;
    }
        
    if (ret < 0) 
    {
        int flag = disconnect(fd, errno);  //判断是否是因为断开连接
        if (flag) 
        {
            epoll_Ctl(fd, EPOLL_CTL_DEL);
        }
    }

    ifstream fin(job.base.from);
    if (!fin)    
    {
        cout << "无此文件" << endl;
        return;
    }
    struct stat s;
    stat(job.base.from, &s);   //获取文件的大小
    int size = s.st_size;

    int tmp = job.base.num;   //分成num个任务块
    for (int i = 0; i < tmp; i++) 
    {
        job.inFo.Id = i;
        job.inFo.clientFd = fd;
        job.inFo.Size = size;
        job.inFo.Location = size * job.inFo.Id / job.base.num;
        job.inFo.writen = job.inFo.Location;
        job.inFo.Bytes = size / job.base.num;
        threadPool.addTask(job);        //添加任务到任务队列
    }
}

void Epoll::epollET(int epollFd, epoll_event* events, int ret) 
{
    for (int i = 0; i < ret; i++) 
    {
        if (events[i].events & EPOLLIN ) //有EPOLLIN事件
        {
            if (events[i].data.fd == listenfd)  //是新的连接请求
            {
                int connfd = newConnect(listenfd);
                cout << "有新连接" << connfd << endl;
                epoll_Ctl(connfd, EPOLL_CTL_ADD);   //将新的连接socketfd添加到合集
            }
            else    //是下载请求
            {
                assignedTask(events[i].data.fd);
            }
        }
        else if (events[i].events & EPOLLRDHUP)     //是客户端断开连接 
        {
            disconnect(events[i].data.fd, errno);
        }
    }
}