#pragma once

class IMotorController
{
public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void setTargetRPM(float rpm) = 0;

    virtual bool isRunning() const = 0;
    virtual float getCurrentRPM() const = 0;
    virtual bool resetFault() = 0;
    virtual String getFault() const = 0;
    virtual ~IMotorController() = default;
};