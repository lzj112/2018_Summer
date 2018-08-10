#include "TimerWheel.h"
#include <sys/time.h>
using namespace std;

TimeWheel::~TimeWheel() 
{
    bool flag = false;
    for (int i = 0; i < N; i++) //每一个槽
    {
        auto it = slots[i].begin();
        auto end = slots[i].end();
        for (; it != end; it++) //槽上每一个定时器
        {
            delete *it;
            *it = nullptr;
            flag = true;
            break;
        }
        if (flag) 
        {
            break;
        }
    }
}

void TimeWheel::addTimer(Timer* timer)  
{
// cout << "here is add Timer" << endl;
    struct timeval tv;
    gettimeofday(&tv, nullptr); 
    time_t siTmp = tv.tv_sec + tv.tv_usec / 1000000 + Si;
    if (timer->clock < 0) 
    {
        return ;
    }
    if (timer->clock < siTmp) //小于Si, 折合为转一圈
    {
        timer->rotation = 1;
    }
    else 
    {
        timer->rotation = timer->clock / siTmp;
    }
    timer->ts = (currentSlot + timer->rotation) % N;   //放到ts槽中
    slots[timer->ts].push_back(timer);
}

void TimeWheel::delTimer(Timer* timer) 
{
// cout << "here is deltimer" << endl;
    bool flag = false;
    if (!timer) 
    {
        return ;
    }
    int ts = timer->ts;
    auto it = slots[ts].begin();
    auto end = slots[ts].end();
    for (; it != end; it++) //在槽ts找到该定时器
    {
        if (*it == timer) 
        {
            flag = true;
            slots[ts].erase(it);    //删除
            break;
        }
    }
    if (flag)
    {
        delete *it;
        *it = nullptr;
    }
}

void TimeWheel::getTime(time_t& time) 
{
    struct timeval tv;
    gettimeofday(&tv, nullptr); 
    time = tv.tv_sec + tv.tv_usec / 1000000 + 20;  //定时时间30s
}

void TimeWheel::adjust(int fd) 
{
cout << "herer is adjust" << endl;
    bool flag = false;
    list<Timer*>::iterator it;
    for (int i = 0; i < N; i++) //每个槽
    {
        it = slots[i].begin();
        auto end = slots[i].end();
        for (; it != end; it++) //槽中每个定时器
        {
            if ((*it)->clientFd == fd) 
            {
                slots[i].erase(it);
                flag = true;
                break;
            }
        }
        if (flag) 
        {
            break;
        }
    }
    if (flag)
    {
        getTime((*it)->clock);  //更新该定时器时间
        addTimer((*it));    //重新添加该定时器
    }
}

void TimeWheel::tick() 
{
    auto it = slots[currentSlot].begin();
    auto end = slots[currentSlot].end();

    struct timeval tv;
    gettimeofday(&tv, nullptr); 
    time_t nowTime = tv.tv_sec + tv.tv_usec / 1000000;  //定时时间
    for (; it != end; it++)    //遍历当前槽的定时器链表
    {
        if ((*it)->rotation > 0)    //该定时器还要再转一圈 
        {
            (*it)->rotation--;
        }
        else 
        {
            if ((*it)->clock < nowTime)
            {
                (*it)->func();
                it = slots[currentSlot].erase(it);
            }
        }
    }
    currentSlot = ++currentSlot % N;
}