#pragma once
#include <Arduino.h>
#include"../MotorDriver/IMotorDriver.h"

class Motor{
  private:
    IMotorDriver& driver;
    uint8_t pwm;
    bool enabled;
  public:
    Motor(IMotorDriver& driver);
    void setPWM(int pwm);
    uint8_t getPWM()const;
    void enable();
    void disable();
    bool isEnabled() const;
    void stop();
};