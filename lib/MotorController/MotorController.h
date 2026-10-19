#pragma once
#include "Motor.h"
#include "Encoder.h"
#include "PIDController.h"
#include "RampGenerator.h"

enum class MotorState{
  Idle,Running,Stopping,EmergencyStop,Fault
};
enum class MotorFault
{
    None,
    EncoderFailure,
    MotorStall,
    OverTemperature,
    DriverFailure
};
class MotorController {
private:

  Motor& motor;
  Encoder& encoder;
  PIDController& pid;
  RampGenerator& ramp;

  MotorState state;
  MotorFault currentFault = MotorFault::None;
  float targetRPM;

  unsigned long faultStartTime = 0;
  bool faultTiming = false;
  bool checkFault(float pwm, float actualRPM,unsigned long now);

public:
  MotorController(
    Motor& motor,
    Encoder& encoder,
    PIDController& pid,
    RampGenerator& ramp);

  void start(float rpm);
  void stop();
  void emergencyStop();
  void resetEmergency();
  void enterFault(MotorFault fault);
  MotorFault getFault() const;
  bool MotorController::resetFault();
  bool isSafeToReset() const;
  void update(float dt);
  String getStatus()const;
};