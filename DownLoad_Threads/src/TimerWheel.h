#ifndef TIMERWHEEL_H
#define TIMERWHEEL_H

#include "TimerClass.h"
#include <array>
#include <list>


class TimeWheel 
{
private:
    static const int N = 3;//= 10;    //10个槽
    static const int Si = 3;    //隔一秒转一次
    std::array<std::list<Timer*>, N> slots; //时间轮 N个槽, Si秒转一次
    int currentSlot;   //当前槽
public:
    TimeWheel() : currentSlot(0){};
    ~TimeWheel();
    int getSi(){
        return Si;
    }
    void addTimer(Timer* timer);   //添加定时器
    void delTimer(Timer* timer);
    void adjust(int fd);    //socket连接有活动,重置其时间将其插入到合适的位置
    void tick();
    void getTime(time_t& time);
};
#endif