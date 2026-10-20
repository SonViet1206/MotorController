#pragma once
#include <stdint.h>
#include <cmath>
#include <Arduino.h>
enum class MotorFault
{
    None,
    Stall,
    Overspeed
};

class FaultManager
{
private:
    MotorFault fault = MotorFault::None;

    unsigned long stallStartTime = 0;
    bool stallTiming = false;

    float minTargetRPM;
    float minActualRPM;
    float minPWM;

    unsigned long stallTimeout;

public:
    FaultManager(
        float minTargetRPM,
        float minActualRPM,
        float minPWM,
        unsigned long stallTimeout);

    void update(
        float targetRPM,
        float actualRPM,
        float pwm,
        unsigned long now);

    bool hasFault() const;

    MotorFault getFault() const;
    String getFaultString() const;

    void reset();
};