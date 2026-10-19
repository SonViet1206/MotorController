#pragma once
#include"IMotorDriver.h"
#include <Arduino.h>

class L298Driver:public IMotorDriver{

    private:
    uint8_t pinA,pinB,pinPWM;
    public:
    explicit L298Driver(uint8_t pinA,uint8_t pinB,uint8_t pinPWM);
    void setPWM(int pwm)override;
};