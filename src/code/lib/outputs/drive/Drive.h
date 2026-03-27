/**
 * @file Drive.h
 * @brief Class containing methods for omnidirectional movement with 4 motors
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#ifndef DRIVE_H
#define DRIVE_H

#include <Arduino.h>

class Drive{
  public:
    Drive();
    void driveToAngle(int angle, int power, int rotation = 0);
    void writeMotorOutput(int motor, int power);
    void writeAllMotorsOutput(int power);
    void brake();
  private:
    const int pwmFowardPins[4] = {14, 36, 11, 9};
    const int pwmReversePins[4] = {15, 37, 12, 10};
    static const int fr = 0, br = 1, bl = 2, fl = 3;
    float scale = 0;
    static const int minPWM = 10;
};

#endif