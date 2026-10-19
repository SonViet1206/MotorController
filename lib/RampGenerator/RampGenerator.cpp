#include"RampGenerator.h"
RampGenerator::RampGenerator(float rate):rate(rate>0?rate:0),currentValue(0){

}
float RampGenerator::update(float target, float dt){
  if(dt<=0) return this->currentValue;
  float maxChange=rate*dt;
  float error=target-currentValue;
  if(error>maxChange){
    currentValue+=maxChange;
  }
  else if(error<-maxChange){
    currentValue-=maxChange;
  }
  else{
    currentValue = target;
  }

  return currentValue;
}

void RampGenerator::reset(float value) {
  currentValue=value;
}
float RampGenerator::getValue()const{
  return currentValue;
}
