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
    void driveToAngle(int angle, int power, int rotation);
    void writeMotorOutput(int motor, int power);
    void writeAllMotorsOutput(int power);
  private:
    const int pwmPins[4] = {0, 0, 0, 0};
    const int in1Pins[4] = {0, 0, 0, 0};
    const int in2Pins[4] = {0, 0, 0, 0};
    static const int fr = 0, br = 1, bl = 2, fl = 3;
    float scale = 0;
};

#endif