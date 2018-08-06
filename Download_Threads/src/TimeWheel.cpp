#include "TimeWheel.h"
using namespace std;

void timerWheel::addTimer(Timer* tmp) 
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