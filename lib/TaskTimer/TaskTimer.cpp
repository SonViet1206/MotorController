#include"TaskTimer.h"

TaskTimer::TaskTimer(uint32_t interval) : interval(interval), previousTime(0), firstUpdate(true)
{
}
void TaskTimer::setInterval(uint32_t interval)
{
    this->interval = interval;
}
void TaskTimer::reset(uint32_t now)
{
    previousTime = now;
    firstUpdate = false;
}
bool TaskTimer::isReady(uint32_t now)
{
    if (firstUpdate)
    {
        firstUpdate = false;
        previousTime = now;
        return false;
    }
    if (now - previousTime >= interval)
    {
        previousTime = now;
        return true;
    }
    return false;
}
