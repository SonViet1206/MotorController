#include"Motor.h"
#include"Arduino.h"

Motor::Motor(IMotorDriver& driver):driver(driver),pwm(0),enabled(false){
  driver.setPWM(0);
}

void Motor::setPWM(int16_t pwm)
{
  this->pwm=constrain(pwm,-255,255);
  if(!enabled){
    driver.setPWM(0);
  }
  else{
    driver.setPWM(this->pwm);
  }
  
}
void Motor::enable(){
  enabled=true;

}
void Motor::disable(){
  enabled=false;
  pwm=0;
  driver.setPWM(pwm);
  
}
void Motor::stop(){
  setPWM(0);
}

int16_t Motor::getPWM() const{
  return pwm;
}
bool Motor::isEnabled() const{
  return enabled;
}