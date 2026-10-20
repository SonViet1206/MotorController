#include"FaultManager.h"

FaultManager::FaultManager(
    float minTargetRPM,
    float minActualRPM,
    float minPWM,
    unsigned long stallTimeout)
    : minTargetRPM(minTargetRPM),
      minActualRPM(minActualRPM),
      minPWM(minPWM),
      stallTimeout(stallTimeout)
{
}

bool FaultManager::hasFault() const
{
    return fault != MotorFault::None;
}

MotorFault FaultManager::getFault() const
{
    return fault;
}

String FaultManager::getFaultString() const
{
    switch (fault)
    {
    case MotorFault::None:
        return "None";
    case MotorFault::Stall:
        return "Stall";
    case MotorFault::Overspeed:
        return "Overspeed";
    default:
        return "Unknown";
    }
}

void FaultManager::reset()
{
    fault = MotorFault::None;
    stallStartTime = 0;
    stallTiming = false;
}

void FaultManager::update(
    float targetRPM,
    float actualRPM,
    float pwm,
    unsigned long now)
{
    if (fault == MotorFault::None)
    {
        if (fabs(targetRPM) >= minTargetRPM && fabs(actualRPM) <= minActualRPM && fabs(pwm) >= minPWM)
        {
            if (!stallTiming)
            {
                stallStartTime = now;
                stallTiming = true;
            }
            else if (now - stallStartTime >= stallTimeout)
            {
                fault = MotorFault::Stall;
            }
        }
        else
        {
            stallTiming = false;
        }
    }
}