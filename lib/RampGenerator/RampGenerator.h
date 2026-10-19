#pragma once
class RampGenerator{
  private:
    float currentValue;
    float rate;
  public:
    explicit RampGenerator(float rate);
    float update(float target,float dt);
    void reset(float value=0);
    float getValue()const;
};