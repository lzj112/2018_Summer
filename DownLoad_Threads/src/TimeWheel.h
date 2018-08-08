#ifndef TIMERWHEEL_H
#define TIMERWHEEL_H

#include "TimerClass.h"
#include <array>
#include <list>


class TimerWheel 
{
private:
    static const int N = 10;    //10个槽
    static const int Si = 1;    //隔一秒转一次
    std::array<std::list<Timer*>, N> slots; //时间轮 N个槽, Si秒转一次
    int currentSlot;   //当前槽
public:
    TimerWheel() : currentSlot(0){};
    ~TimerWheel();
    void addTimer(Timer* tmp);   //添加定时器
    void delTimer(Timer* tmp);
    void adjust(int fd);    //socket连接有活动,重置其时间将其插入到合适的位置
    void tick();
};


#endif