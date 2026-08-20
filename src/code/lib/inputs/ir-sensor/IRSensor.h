/**
 * @file IRSensor.h
 *
 * @brief TSSP-only ball angle + distance detection (raw-count vector method),
 *        refined near the front with a 3-photodiode array for higher angular
 *        precision in a narrow front window.
 *
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#ifndef IRSensor_H
#define IRSensor_H

#define numTSSP 16
#define bufferSize 250

#define numPhotodiodes 3

// How many neighbors each side of the peak sensor to use for angle (7-wide window on 24 sensors -> scaled to 16)
#define ANGLE_HALF_WINDOW 2
// How many total sensors around the peak to average for distance (12-wide window on 24 sensors -> scaled to 16)
#define DIST_BEFORE 2
#define DIST_AFTER  3

#define NOISE_THRESHOLD   15      // min smoothed count to count as "seeing" a sensor (tune for your buffer size)
#define LOST_RESET_COUNT  10     // consecutive "no ball" updates before resetting distance/angle filters
#define ANGLE_EMA_ALPHA   0.1f  // smoothing applied to the angle vector components (not the angle itself)

// --- Photodiode front-refinement ---
// photodiodes[0] sits under tssp[15], photodiodes[1] under tssp[0], photodiodes[2] under tssp[1]
#define PHOTO_FRONT_CENTER_DEG   90.0f   // angular center of the photodiode cluster == direction of tssp[0]
#define PHOTO_WINDOW_HALF_DEG    12.5f   // +-window (in degrees) around the center where photodiodes take over
#define PHOTO_WINDOW_HYSTERESIS  3.0f    // extra degrees required to EXIT the window once inside, prevents edge chatter
#define PHOTO_NOISE_MARGIN       75      // reading must exceed baseline by this much to count as a real detection

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class IRSensor {
  public:
    IRSensor();
    void update(unsigned long timeLimit);
    void printIR(unsigned long timeLimit);

    bool isBallDetected();

    int getAngle();
    int getDistance();
    int getTSSPDetecting();
    float getBallVectorX();
    float getBallVectorY();

  private:
    //constants
    const int tssp[numTSSP] = {33, 34, 35, 36, 37, 14, 15, 3, 2, 4, 5, 6, 12, 30, 32, 31};
    const int photodiodes[numPhotodiodes] = {27, 39, 38};
    // Index into the TSSP direction table (vectorX/vectorY) that each photodiode is physically aligned with
    const int photoDirIndex[numPhotodiodes] = {15, 0, 1};
    // Per-sensor no-ball baseline reading, measured by the user during calibration
    const int photoBaseline[numPhotodiodes] = {100, 50, 60};
    const int photoMaxReading = 370; // approximate saturation reading with ball touching sensor, across all 3

    const int neoPin = 11;
    Adafruit_NeoPixel pixels;

    const float vectorX[numTSSP] = {
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

    const float vectorY[numTSSP] = {
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
    bool tsspDetected[numTSSP][bufferSize]; // matrix with tssp states over time
    int bufferIndex = 0;
    int tsspTimesDetected[numTSSP];         // raw hit-count per sensor in current buffer window

    // history buffer used for the distance smoothing (separate, slower-moving average)
    static const int distHistCount = 12;
    uint32_t sensorHistory[numTSSP][distHistCount];
    int histIndex = 0;
    bool histFull = false;
    uint32_t smoothedCount[numTSSP];

    int photodiodeReadings[numPhotodiodes]; // raw analogRead, unadjusted (kept for printIR/debug)

    int tsspDetecting = 0;
    uint8_t lostCount = 0;

    float filteredX = 0;
    float filteredY = 0;
    int smoothAngle = 500;
    float ballVectorX = 0;
    float ballVectorY = 0;
    bool emaInit = false;
    bool usingPhotoVector = false; // hysteresis state: true while locked onto the photodiode window

    float distance = 254;
    float filteredDistance = 254;

    void updateSensors();
    void updatePhotodiodes();
    bool updateHistoryAndCheckReaction();
    int findMaxSensorIndex();
    int findMaxRawSensorIndex();
    void calculateBallVector();
    bool calculatePhotoVector(float &photoX, float &photoY);
    void calculateDistance(int maxIndex);
    void resetTracking();
};

#endif