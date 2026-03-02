/**
 * @file IRSensor.h
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#ifndef IRSensor_H
#define IRSensor_H

#define numTSSP 18
#define bufferSize 100

#include <Arduino.h>

class IRSensor {
  public:
    IRSensor();
    void update(unsigned long timeLimit);
    void printIR();

    int getAngle();

  private:
    //constants
    const int tssp[numTSSP] = {0, 1, 3, 4, 27, 28, 29, 31, 32, 33, 34, 36,  37, 38, 15, 20, 22, 23};

    const float vectorX[numTSSP] = {
      1.0000000000,  0.9396926208,  0.7660444431,  0.5000000000,  0.1736481777,
      -0.1736481777, -0.5000000000, -0.7660444431, -0.9396926208, -1.0000000000,
      -0.9396926208, -0.7660444431, -0.5000000000, -0.1736481777,  0.1736481777,
      0.5000000000,  0.7660444431,  0.9396926208
    };
    const float vectorY[numTSSP] = {
      0.0000000000,  0.3420201433,  0.6427876097,  0.8660254038,  0.9848077530,  
      0.9848077530,  0.8660254038,  0.6427876097,  0.3420201433,  0.0000000000, 
      -0.3420201433, -0.6427876097, -0.8660254038, -0.9848077530, -0.9848077530,  
      -0.8660254038, -0.6427876097, -0.3420201433
    };

    //variables and locals
    int rawAngle = -1;

    bool tsspDetected[numTSSP][bufferSize]; //matrix with tssp states over time
    int bufferIndex = 0;
    int tsspTimesDetected[numTSSP];
    int consecutiveDetections[numTSSP];
    unsigned long lastDetectionTime[numTSSP];
    
    void updateTSSP();
    void calculateBallVector();
};

#endif