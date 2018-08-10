#ifndef TIMERCLASS_H
#define TIMERCLASS_H 

#include <sys/time.h>
#include <iostream>
#include <functional>
#include <sys/socket.h>
#include <unistd.h>

struct Timer 
{
    time_t clock;   //定时时间  time_t就是long int
    int clientFd;   //客户端sockfd
    int rotation;   //记录定时器在时间轮赚多少圈后生效
    int ts;         //记录定时器在时间轮的哪个槽
    // std::function<void(int)> func;   //定时函数----关闭那些不活跃的sockfd
    Timer(){}
    Timer(int fd) : clientFd(fd)
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr); 
        clock = tv.tv_sec + tv.tv_usec / 1000000 + 10;  //定时时间
    }
    void func() 
    {
        shutdown(clientFd, SHUT_RDWR);
        // close(clientFd);
// std::cout << "SHUTDOWN!" << std::endl;
    }
    
};
#endif