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
#include <functional>
#include <sys/timerfd.h>
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
cout << "here is epoll_ctl " << fd << endl;
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollFd, op, fd, &ev);
}

void Epoll::Stop() //停止程序
{
    stopEpoll = false;
    close(epollFd);
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

void Epoll::addToTimeWheel(int fd) //将新连接fd封装为定时器添加到时间轮
{
    Timer* timer = new Timer(fd/*, &Epoll::shutDown*/);
    // timer->func = bind(&Epoll::shutDownFd, *this, placeholders::_1);
    timer->rotation = 0;
    timeWheel.addTimer(timer);
}

void Epoll::setTimer() //初始化, 设置定时
{
    int ret;
    struct itimerspec newValue;
    struct timespec nowTime;
    ret = clock_gettime(CLOCK_REALTIME, &nowTime);  //得到现在的时间
    assert(ret != -1); 

    newValue.it_value.tv_sec = nowTime.tv_sec + 1;   //第一次到期时间
    newValue.it_value.tv_nsec = nowTime.tv_nsec;
    newValue.it_interval.tv_sec = 3;//timeWheel.getSi();    //时间间隔
    newValue.it_interval.tv_nsec = 0;

    timerFd = timerfd_create(CLOCK_REALTIME, /*0*/TFD_NONBLOCK);   //构建了一个非阻塞的定时器,相对时间
    assert(timerFd != -1);
    ret = timerfd_settime(timerFd, TFD_TIMER_ABSTIME, &newValue, nullptr);
    assert(ret != -1);

    epoll_Ctl(timerFd, EPOLL_CTL_ADD);  //将指代该定时器对象的文件描述符添加进epoll事件合集
}

void Epoll::epoll_Run() 
{
    int ret;
    signal(SIGINT, SIG_IGN);    //忽略软中断
    signal(SIGPIPE, SIG_IGN);   //忽略sigpipe 
    setTimer();
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

void Epoll::acceptPackage(int fd, DownloadMsg& job) 
{
    int ret = 0;
    int sum = 0, len = 0, count = 0;
    while (sum < sizeof(job.head))   //先收包头
    {
        ret = recv(fd, (void*)(&job + sum), sizeof(job.head), 0);
        if (ret <= 0) 
        {
            break;
        }
        sum += ret;
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
    sum = 0, ret = 0;
    len = job.head.packetLength;
    while (sum < len)      //收包体
    {
        ret = recv(fd, (void*)(&job.body + sum), sizeof(job.body), 0);
        if (ret <= 0) 
        {
            break;
        }
        sum += ret;
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
}

void Epoll::assignedTask(int fd) //读取客户端的下载请求并分配任务
{
    DownloadMsg job;
    acceptPackage(fd, job);   //接收协议体

    ifstream fin(job.body.base.from);
    if (!fin)    
    {
        cout << "无此文件" << endl;
        return;
    }
    struct stat s;
    stat(job.body.base.from, &s);   //获取文件的大小
    int size = s.st_size;

    int tmp = job.body.base.num;   //分成num个任务块
    job.head.serviceType = job.head.serviceType; //等于1时为断点续传, -1时为普通下载请求
    for (int i = 0; i < tmp; i++) 
    {
        job.body.inFo.Id = i;
        job.body.inFo.clientFd = fd;
        job.body.inFo.Size = size;
        job.body.inFo.Location = size * job.body.inFo.Id / job.body.base.num;
        job.body.inFo.writen = job.body.inFo.Location;
        job.body.inFo.Bytes = size / job.body.base.num;
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
                addToTimeWheel(connfd);  //更新socket活跃度
            }
            else if (events[i].data.fd != timerFd)    //是下载请求
            {
                timeWheel.adjust(events[i].data.fd);
                assignedTask(events[i].data.fd);    //解析下载请求
            }
            else
            {
                uint64_t numExp;
                ssize_t s = read(events[i].data.fd, &numExp, sizeof(uint64_t));
                if (s == sizeof(uint64_t)) 
                timeWheel.tick();   //指定定时任务
cout << "here is after tick()" << endl;
            }
        }
        else if (events[i].events & EPOLLRDHUP)     //是客户端断开连接 
        {
            disconnect(events[i].data.fd, errno);
        }
    }
}

