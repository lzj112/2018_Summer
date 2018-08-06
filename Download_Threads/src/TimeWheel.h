#ifndef TIMERWHEEL_H
#define TIMERWHEEL_H

#include "TimerClass.h"
#include <array>
#include <list>


class timerWheel 
{
private:
    static const int N = 60;
    static const int Si = 1;
    std::array<std::list<Timer*>, N> slots; //时间轮 N个槽, Si秒转一次
    int cur_slot;   //当前槽
public:
    timerWheel() : cur_slot(0){};
    ~timerWheel();
    void addTimer(Timer* tmp);   //添加定时器
    void delTimer(Timer* tmp);
    void adjust(int fd);    //socket连接有活动,重置其时间将其插入到合适的位置
    void tick();
};


#endif