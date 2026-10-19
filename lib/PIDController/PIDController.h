#pragma once
class PIDController{
  private:
    float kp, ki, kd;
    float integral = 0;
    float previousError = 0;
    float previousMeasurement=0;
    float filteredDerivative=0;
    float outputMin, outputMax;
    float alpha;
    bool firstUpdate;
    float lastOutput;
  public:
    PIDController(float kp, float ki, float kd,float minOutput, float maxOutput,float alpha=0.9f);
    float calculate(float target,float actual,float dt);
    void reset();
    void setTunings(float kp, float ki, float kd);

};