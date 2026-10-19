#include "Arduino.h"
#include "SpeedController.h"

SpeedController::SpeedController(Motor& motor,Encoder& encoder,PIDController& pid,unsigned long interval,RampGenerator& ramp): 
                                  motor(motor), encoder(encoder), pid(pid),INTERVAL(interval),ramp(ramp) {

}

void SpeedController::setTargetRPM(float rpm) {
  targetRPM = rpm;

}
void SpeedController::update(unsigned long now){
  
  if (previousTime == 0) {
    previousTime = now;
    encoder.reset();
    return;
  }

  unsigned long elapsed = now - previousTime;
 
  if (elapsed < INTERVAL) return;

  float dt = elapsed / 1000.0f;
  previousTime = now;
  currentRPM = encoder.getRPM(dt);
  float smoothTarget =ramp.update(targetRPM,dt);
  float output = pid.calculate(smoothTarget, currentRPM, dt);
  motor.setPWM((int)output);   

  Serial.print("Target:");
  Serial.print(targetRPM);
  Serial.print(",Actual:");
  Serial.print(currentRPM);
  Serial.print(",PWM:");
  Serial.println((int)output);  

}
float SpeedController::getCurrentRPM() const {
  return currentRPM;
}