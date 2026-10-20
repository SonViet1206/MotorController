#pragma once
#include <Arduino.h>
#include"IMotorDriver.h"

class Motor{
  private:
    IMotorDriver& driver;
    int16_t pwm;
    bool enabled;
  public:
    Motor(IMotorDriver& driver);
    void setPWM(int16_t pwm);
    int16_t getPWM()const;
    void enable();
    void disable();
    bool isEnabled() const;
    void stop();
};