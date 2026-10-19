#include"L298Driver.h"
#include <Arduino.h>

L298Driver::L298Driver(uint8_t pinA,uint8_t pinB,uint8_t pinPWM):pinA(pinA),pinB(pinB),pinPWM(pinPWM){
    pinMode(pinA,OUTPUT);
    pinMode(pinB,OUTPUT);
    pinMode(pinPWM,OUTPUT);
    digitalWrite(pinA,LOW);
    digitalWrite(pinB,LOW);
    digitalWrite(pinPWM,LOW);
}
void L298Driver::setPWM(int pwm) {
    if(pwm>0){
        digitalWrite(pinA,HIGH);
        digitalWrite(pinB,LOW);
    }
    else if(pwm<0){
        digitalWrite(pinB,HIGH);
        digitalWrite(pinA,LOW);
    }
    else{
        digitalWrite(pinB,LOW);
        digitalWrite(pinA,LOW);
    }
    analogWrite(pinPWM,abs(pwm));
}