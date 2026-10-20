#pragma once
#include "Motor.h"
#include "Encoder.h"
#include "PIDController.h"
#include "RampGenerator.h"
#include"FaultManager.h"
#include <IMotorController.h>

class SpeedController : public IMotorController{
  private:
    Motor& motor;
    Encoder& encoder;
    PIDController& pid; 
    RampGenerator& ramp;
    FaultManager& faultManager;

    float targetRPM = 0;
    float currentRPM = 0;
    unsigned long previousTime = 0;

    unsigned long INTERVAL = 100;
    bool firstUpdate = true;
    bool running = false;

  public:
    SpeedController(Motor& motor,Encoder& encoder,PIDController& pid,RampGenerator& ramp,FaultManager& faultManager,unsigned long interval);
    void setTargetRPM(float rpm)override;
    void update(unsigned long now);
    float getCurrentRPM() const;
    void start() override;
    void stop() override;
    bool isRunning() const override;
    bool resetFault() override;
    String getFault() const override;

};