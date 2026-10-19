#pragma once

class IMotorDriver
{
public:
    virtual void setPWM(int pwm) = 0;

    virtual ~IMotorDriver() = default;
};