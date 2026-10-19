#include "PIDController.h"

PIDController::PIDController(float kp, float ki, float kd,float minOutput, float maxOutput,float alpha):
 kp(kp), ki(ki), kd(kd),outputMin(minOutput),outputMax(maxOutput),alpha(alpha),firstUpdate(true) {
}

float PIDController::calculate(float target, float actual, float dt){
  if(dt<=0) return lastOutput;
  float err=target-actual;
  float p=kp*err;
  //tính đạo hàm theo measurement
  float derivative = 0;
  if(firstUpdate)
  {
    previousMeasurement=actual;
    firstUpdate = false;
  }
  else
  {
    derivative =-(actual-previousMeasurement)/dt;
  }
  //--lọc đạo hàm
  filteredDerivative=(alpha*filteredDerivative)+(1.0f-alpha)*derivative;
  float d=kd*filteredDerivative;

  //Xử lý I Chống Integral Windup
  float newIntegral = integral+err*dt;

  float rawOutput=p+ki*newIntegral+d;//Tính PID

  // Chống tích lũy sai số khi bão hòa PWM
  if (!((rawOutput > outputMax && err > 0) ||(rawOutput < outputMin && err < 0))) {
    integral = newIntegral;
  }
  float output = p + ki * integral + d;
  if(output>outputMax)output=outputMax;
  if(output<outputMin) output=outputMin;

  previousError=err;
  previousMeasurement=actual;
  lastOutput=output;
  return output;

}
void PIDController::reset() {
    integral = 0;
    previousError = 0;
    previousMeasurement = 0;
    filteredDerivative = 0;
    firstUpdate = true;
}
void PIDController::setTunings(float p, float i, float d)
{
  kp = p;
  ki = i;
  kd = d;
}