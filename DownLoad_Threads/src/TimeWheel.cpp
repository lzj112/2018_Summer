#include "TimeWheel.h"
using namespace std;

TimerWheel::~TimerWheel() 
{
    for (int i = 0; i < N; i++) 
    {
        auto it = slots[i].begin();
        auto end = slots.end();
        for (; it != end; it++) 
        {
            delete *it;
        }
    }
}

void TimerWheel::addTimer(Timer* tmp) 
{
    if (tmp->clock < 0) 
    {
        return ;
    }
    int ticks = 0;
    if (tmp->clock < Si) 
    {
        ticks = 0;
    }
    else 
    {
        ticks = tmp->clock / Si;
    }
    tmp->rotation = ticks / N;

}