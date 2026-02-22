/**
 * @file PD.h
 * @brief Logic for pd controller
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#ifndef PD_H
#define PD_H

#include <Arduino.h>

class PD {
  private:
    double error = 0;
    double prevError = 0;
    double kP = 0;
    double kI = 0;
    double kD = 0;
    int maxPow = 0;
    double deltaTime = 0.01;
  
  public:
    PD(double kp, double kd, int maxpow, double deltatime = 0.01){
      kP = kp; kD = kd; maxPow = maxpow; deltaTime = deltatime;
    };
    int getCorrection(double error){
      double derivative = (error - prevError) / deltaTime;
      int correction = (int) (kP * error) + (kD * derivative);
      correction = constrain(correction, -maxPow, maxPow);
      prevError = error;
      return correction;
    };
};

#endif