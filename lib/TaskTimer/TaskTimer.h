#pragma once
#include <stdint.h>

class TaskTimer
{
private:
    uint32_t interval;
    uint32_t previousTime;
    bool firstUpdate;

public:
    explicit TaskTimer(uint32_t interval);

    bool isReady(uint32_t now);

    void reset(uint32_t now);

    void setInterval(uint32_t interval);
};