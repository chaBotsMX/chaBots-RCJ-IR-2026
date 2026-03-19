/**
 * @file Kicker.h
 * @brief Class containing logic to open solenoid's circuit in a cooldown
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#ifndef KICKER_H
#define KICKER_H

#include <Arduino.h>

class Kicker {
  private:
    const int solenoid = 32;
    unsigned long intervalTimer = 0;
    unsigned long kickingTimer = 0;
    bool isActive = false;
  public:
    Kicker(){
      pinMode(solenoid, OUTPUT);
    };

    void kick(){
      if(!isActive && millis() - intervalTimer >= 3000){
        digitalWrite(solenoid, HIGH);
        intervalTimer = millis();
        kickingTimer = millis();
        isActive = true;
      }
    }

    void update(){
      if(isActive && (millis() - kickingTimer >= 10)){
        digitalWrite(solenoid, LOW);
        isActive = false;
      }
    }
};

#endif