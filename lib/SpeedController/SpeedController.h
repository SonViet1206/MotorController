#pragma once
#include "Motor.h"
#include "Encoder.h"
#include "PIDController.h"
#include "RampGenerator.h"
class SpeedController{
  private:
    Motor& motor;
    Encoder& encoder;
    PIDController& pid; 
    RampGenerator& ramp;
    float targetRPM = 0;
    float currentRPM = 0;
    unsigned long previousTime = 0;

     unsigned long INTERVAL = 100;
  public:
    SpeedController(Motor& motor,Encoder& encoder,PIDController& pid,unsigned long interval,RampGenerator& ramp);
    void setTargetRPM(float rpm);
    void update(unsigned long now);
    float getCurrentRPM() const;
};