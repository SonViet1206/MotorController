#include "Arduino.h"
#include "SpeedController.h"

SpeedController::SpeedController(
    Motor &motor,
    Encoder &encoder,
    PIDController &pid,
    RampGenerator &ramp,
    FaultManager &faultManager,
    unsigned long interval) : motor(motor),
                              encoder(encoder), pid(pid), ramp(ramp),
                              faultManager(faultManager), INTERVAL(interval)
{
}

void SpeedController::setTargetRPM(float rpm)
{
  targetRPM = rpm;
}
void SpeedController::update(unsigned long now)
{
  if (!running)
    return;

  if (firstUpdate)
  {
    firstUpdate = false;
    previousTime = now;
    encoder.reset();
    return;
  }

  unsigned long elapsed = now - previousTime;

  if (elapsed < INTERVAL)
    return;

  float dt = elapsed / 1000.0f;
  previousTime = now;
  encoder.calculateRPM(dt);
  currentRPM = currentRPM = encoder.getRPM();

  float smoothTarget = ramp.update(targetRPM, dt);
  float output = pid.calculate(smoothTarget, currentRPM, dt);

  faultManager.update(smoothTarget, currentRPM, output, now);
  if (faultManager.hasFault())
  {
    stop();
  }
  motor.setPWM((int)output);
}
float SpeedController::getCurrentRPM() const
{
  return currentRPM;
}
void SpeedController::start()
{
  if (faultManager.hasFault())
    return;
  if (running)
    return;

  pid.reset();
  ramp.reset(0);

  targetRPM = 0;
  motor.setPWM(0);
  motor.enable();

  running = true;
}

void SpeedController::stop()
{
  running = false;
  targetRPM = 0;

  pid.reset();
  ramp.reset(0);

  motor.disable();
}
bool SpeedController::resetFault()
{
  if (running||faultManager.hasFault()==false)
    return false;

  motor.disable();
  targetRPM = 0;

  pid.reset();
  ramp.reset(0);
  faultManager.reset();
  return true;
}
String SpeedController::getFault() const
{
  return faultManager.getFaultString();
}