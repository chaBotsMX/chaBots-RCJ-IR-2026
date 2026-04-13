/**
 * @file IRSensor.h
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#ifndef LINESENSOR_H
#define LINESENSOR_H

#define numSensors 32

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class LineSensor {
  public:
    LineSensor();
    void begin();
    void update();
    void printLS(unsigned long timeLimit);
    int getAngle();

  private:
    const int neoPin = 10;
    const int comparators[numSensors] = {36, 29, 30, 31, 32, 27, 26, 25, 24, 9, 8, 7, 6, 0, 1, 2, 3, 20, 21, 22, 23, 19, 18, 17, 16, 40, 39, 38, 37, 33, 34, 35};
    const int vref[4] = {4, 12, 13, 28};
    Adafruit_NeoPixel pixels;
    bool readings[numSensors];
    const float vectorX[32] = {
      0.0000000000,   // Index 0:  cos(90.0°)
      0.1950903220,   // Index 1:  cos(78.75°)
      0.3826834324,   // Index 2:  cos(67.5°)
      0.5555702330,   // Index 3:  cos(56.25°)
      0.7071067812,   // Index 4:  cos(45.0°)
      0.8314696123,   // Index 5:  cos(33.75°)
      0.9238795325,   // Index 6:  cos(22.5°)
      0.9807852804,   // Index 7:  cos(11.25°)
      1.0000000000,   // Index 8:  cos(0.0°)
      0.9807852804,   // Index 9:  cos(-11.25°)
      0.9238795325,   // Index 10: cos(-22.5°)
      0.8314696123,   // Index 11: cos(-33.75°)
      0.7071067812,   // Index 12: cos(-45.0°)
      0.5555702330,   // Index 13: cos(-56.25°)
      0.3826834324,   // Index 14: cos(-67.5°)
      0.1950903220,   // Index 15: cos(-78.75°)
      0.0000000000,   // Index 16: cos(-90.0°)
      -0.1950903220,  // Index 17: cos(-101.25°)
      -0.3826834324,  // Index 18: cos(-112.5°)
      -0.5555702330,  // Index 19: cos(-123.75°)
      -0.7071067812,  // Index 20: cos(-135.0°)
      -0.8314696123,  // Index 21: cos(-146.25°)
      -0.9238795325,  // Index 22: cos(-157.5°)
      -0.9807852804,  // Index 23: cos(-168.75°)
      -1.0000000000,  // Index 24: cos(-180.0°)
      -0.9807852804,  // Index 25: cos(-191.25°)
      -0.9238795325,  // Index 26: cos(-202.5°)
      -0.8314696123,  // Index 27: cos(-213.75°)
      -0.7071067812,  // Index 28: cos(-225.0°)
      -0.5555702330,  // Index 29: cos(-236.25°)
      -0.3826834324,  // Index 30: cos(-247.5°)
      -0.1950903220   // Index 31: cos(-258.75°)
    };

    const float vectorY[32] = {
      1.0000000000,   // Index 0:  sin(90.0°)
      0.9807852804,   // Index 1:  sin(78.75°)
      0.9238795325,   // Index 2:  sin(67.5°)
      0.8314696123,   // Index 3:  sin(56.25°)
      0.7071067812,   // Index 4:  sin(45.0°)
      0.5555702330,   // Index 5:  sin(33.75°)
      0.3826834324,   // Index 6:  sin(22.5°)
      0.1950903220,   // Index 7:  sin(11.25°)
      0.0000000000,   // Index 8:  sin(0.0°)
      -0.1950903220,  // Index 9:  sin(-11.25°)
      -0.3826834324,  // Index 10: sin(-22.5°)
      -0.5555702330,  // Index 11: sin(-33.75°)
      -0.7071067812,  // Index 12: sin(-45.0°)
      -0.8314696123,  // Index 13: sin(-56.25°)
      -0.9238795325,  // Index 14: sin(-67.5°)
      -0.9807852804,  // Index 15: sin(-78.75°)
      -1.0000000000,  // Index 16: sin(-90.0°)
      -0.9807852804,  // Index 17: sin(-101.25°)
      -0.9238795325,  // Index 18: sin(-112.5°)
      -0.8314696123,  // Index 19: sin(-123.75°)
      -0.7071067812,  // Index 20: sin(-135.0°)
      -0.5555702330,  // Index 21: sin(-146.25°)
      -0.3826834324,  // Index 22: sin(-157.5°)
      -0.1950903220,  // Index 23: sin(-168.75°)
      0.0000000000,   // Index 24: sin(-180.0°)
      0.1950903220,   // Index 25: sin(-191.25°)
      0.3826834324,   // Index 26: sin(-202.5°)
      0.5555702330,   // Index 27: sin(-213.75°)
      0.7071067812,   // Index 28: sin(-225.0°)
      0.8314696123,   // Index 29: sin(-236.25°)
      0.9238795325,   // Index 30: sin(-247.5°)
      0.9807852804    // Index 31: sin(-258.75°)
    };
    void calculateLineVector();
    int angle = 0;
    float sumX = 0;
    float sumY = 0;
};

#endif