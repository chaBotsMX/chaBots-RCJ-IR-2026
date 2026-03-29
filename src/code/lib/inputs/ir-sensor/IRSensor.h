/**
 * @file IRSensor.h
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#ifndef IRSensor_H
#define IRSensor_H

#define numSensors 16
#define bufferSize 60

#include <Arduino.h>

class IRSensor {
  public:
    IRSensor();
    void update(unsigned long timeLimit);
    void printIR(unsigned long timeLimit);

    bool isBallDetected();
    bool usingTSSP();

    int getAngle();
    int getDistance();

  private:
    //constants
    const int tssp[numSensors] = {33, 34, 35, 36, 37, 14, 15, 3, 2, 4, 5, 6, 12, 30, 32, 31};
    const int photodiodes[numSensors] = {39, 38, 40, 41, 16, 17, 18, 19, 22, 23, 21, 20, 24, 25, 26, 27};

    const float vectorX[numSensors] = {
      0.0000000000,   // Index 0:  cos(90°)
      0.3826834324,   // Index 1:  cos(67.5°)
      0.7071067812,   // Index 2:  cos(45°)
      0.9238795325,   // Index 3:  cos(22.5°)
      1.0000000000,   // Index 4:  cos(0°)
      0.9238795325,   // Index 5:  cos(-22.5°)
      0.7071067812,   // Index 6:  cos(-45°)
      0.3826834324,   // Index 7:  cos(-67.5°)
      0.0000000000,   // Index 8:  cos(-90°)
      -0.3826834324,  // Index 9:  cos(-112.5°)
      -0.7071067812,  // Index 10: cos(-135°)
      -0.9238795325,  // Index 11: cos(-157.5°)
      -1.0000000000,  // Index 12: cos(-180°)
      -0.9238795325,  // Index 13: cos(-202.5°)
      -0.7071067812,  // Index 14: cos(-225°)
      -0.3826834324   // Index 15: cos(-247.5°)
    };

    const float vectorY[numSensors] = {
      1.0000000000,   // Index 0:  sin(90°)
      0.9238795325,   // Index 1:  sin(67.5°)
      0.7071067812,   // Index 2:  sin(45°)
      0.3826834324,   // Index 3:  sin(22.5°)
      0.0000000000,   // Index 4:  sin(0°)
      -0.3826834324,  // Index 5:  sin(-22.5°)
      -0.7071067812,  // Index 6:  sin(-45°)
      -0.9238795325,  // Index 7:  sin(-67.5°)
      -1.0000000000,  // Index 8:  sin(-90°)
      -0.9238795325,  // Index 9:  sin(-112.5°)
      -0.7071067812,  // Index 10: sin(-135°)
      -0.3826834324,  // Index 11: sin(-157.5°)
      0.0000000000,   // Index 12: sin(-180°)
      0.3826834324,   // Index 13: sin(-202.5°)
      0.7071067812,   // Index 14: sin(-225°)
      0.9238795325    // Index 15: sin(-247.5°)
    };

    //variables and locals
    int rawAngle = 500;
    int magnitude = -1;

    bool tsspDetected[numSensors][bufferSize]; //matrix with tssp states over time
    int photodiodeReadings[numSensors];
    int bufferIndex = 0;
    int tsspTimesDetected[numSensors];
    int consecutiveDetections[numSensors];

    float filteredX = 0;
    float filteredY = 0;
    float filterAlpha = 0.2; // Adjust between 0.0 and 1.0 (Lower = smoother but laggier)
    int smoothAngle = 500;
    
    void updateTSSP();
    void updatePhotodiodes();
    void calculateBallVector();
};

#endif