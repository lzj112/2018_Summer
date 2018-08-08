#ifndef TIMERCLASS_H
#define TIMERCLASS_H 

#include <sys/time.h>
#include <iostream>
#include <functional>
class Timer 
{
public:
    time_t clock;   //定时时间  time_t就是long int
    int clientFd;   //客户端sockfd
    int rotation;   //记录定时器在时间轮赚多少圈后生效
    std::function<void()> func;   //定时函数----关闭那些不活跃的sockfd
    Timer(){}
    Timer(int fd, std::function<void()>& f) : clientFd(fd), func(f) 
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr); 
        clock = tv.tv_sec + tv.tv_usec / 1000000 + 30;  //定时时间30s
    }
    
};
#endif