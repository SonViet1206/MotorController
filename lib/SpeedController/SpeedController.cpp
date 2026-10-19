#include "Arduino.h"
#include "SpeedController.h"

SpeedController::SpeedController(Motor &motor, Encoder &encoder, PIDController &pid, unsigned long interval, RampGenerator &ramp) : motor(motor), encoder(encoder), pid(pid), INTERVAL(interval), ramp(ramp)
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
  motor.setPWM((int)output);

  Serial.print("Target:");
  Serial.print(targetRPM);
  Serial.print(",Actual:");
  Serial.print(currentRPM);
  Serial.print(",PWM:");
  Serial.println((int)output);
}
float SpeedController::getCurrentRPM() const
{
  return currentRPM;
}
void SpeedController::start()
{
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