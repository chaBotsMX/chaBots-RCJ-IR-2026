/**
 * @file IRSensor.cpp
 * @brief TSSP-only ball angle + distance detection, ported from the raw hit-count
 *        vector method (peak-window weighted atan2 for angle, peak-window average
 *        of smoothed hit-counts for distance). Refined near the front with a
 *        3-photodiode array blended in based on how centered the ball is in the window.
 *
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#include "IRSensor.h"

IRSensor::IRSensor() : pixels(numTSSP, neoPin, NEO_RGB + NEO_KHZ800){
  for (int i = 0; i < numTSSP; i++) {
    pinMode(tssp[i], INPUT);
  }

  for(int i = 0; i < numTSSP; i++){
    tsspTimesDetected[i] = 0;
    smoothedCount[i] = 0;
  }

  for(int i = 0; i < numPhotodiodes; i++){
    photodiodeReadings[i] = 0;
  }

  memset(tsspDetected, 0, sizeof(tsspDetected));
  memset(sensorHistory, 0, sizeof(sensorHistory));

  analogReadResolution(14);
  analogReadAveraging(16);

  pixels.begin();
  pixels.clear();
}

void IRSensor::update(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;

  updateSensors();
  updatePhotodiodes();
  calculateBallVector(); // uses raw tsspTimesDetected[] -> updates every call, no timer gate

  if((micros() - lastUpdate) >= timeLimit){
    lastUpdate = micros();

    bool ballPresent = updateHistoryAndCheckReaction();

    if(ballPresent){
      lostCount = 0;
      int maxIndex = findMaxSensorIndex(); // smoothed peak, for the slower distance calc
      calculateDistance(maxIndex);
    } else {
      lostCount++;
      if(lostCount >= LOST_RESET_COUNT){
        resetTracking();
      }
    }
  }
}

// Fast buffered sampling of the raw TSSP pins.
// Uses a running count per sensor (O(1) per sensor per call) instead of
// rescanning the whole ring buffer every update (was O(bufferSize) per sensor,
// i.e. numTSSP*bufferSize reads every single call).
void IRSensor::updateSensors(){
  for(int i = 0; i < numTSSP; i++){
    bool currentDetection = !digitalReadFast(tssp[i]);
    bool oldValue = tsspDetected[i][bufferIndex];

    if(currentDetection != oldValue){
      tsspTimesDetected[i] += currentDetection ? 1 : -1;
    }

    tsspDetected[i][bufferIndex] = currentDetection;
  }

  bufferIndex++;
  if(bufferIndex >= bufferSize) bufferIndex = 0;
}

void IRSensor::updatePhotodiodes(){
  for(int i = 0; i < numPhotodiodes; i++){
    photodiodeReadings[i] = analogRead(photodiodes[i]);
  }
}

// Pushes the current raw counts into the distance history buffer and returns
// whether any sensor is currently seeing the ball above the noise threshold.
bool IRSensor::updateHistoryAndCheckReaction(){
  for(int i = 0; i < numTSSP; i++){
    sensorHistory[i][histIndex] = tsspTimesDetected[i];
  }

  int loops = histFull ? distHistCount : (histIndex + 1);

  histIndex++;
  if(histIndex >= distHistCount){
    histIndex = 0;
    histFull = true;
  }

  bool anyReaction = false;
  for(int i = 0; i < numTSSP; i++){
    uint32_t sum = 0;
    for(int h = 0; h < loops; h++){
      sum += sensorHistory[i][h];
    }
    smoothedCount[i] = sum / loops;

    if(smoothedCount[i] > NOISE_THRESHOLD){
      anyReaction = true;
    } else{
      smoothedCount[i] = 0; // zero out weak sensors to avoid noise affecting angle/distance calculations
    }
  }

  return anyReaction;
}

int IRSensor::findMaxSensorIndex(){
  int maxIdx = 0;
  uint32_t maxVal = 0;
  for(int i = 0; i < numTSSP; i++){
    if(smoothedCount[i] > maxVal){
      maxVal = smoothedCount[i];
      maxIdx = i;
    }
  }
  return maxIdx;
}

// Peak finder over the raw, instantaneous per-cycle counts (no history smoothing).
// Used by calculateBallVector() so angle tracks the ball every loop, not just
// on the (slower) distance-update timer.
int IRSensor::findMaxRawSensorIndex(){
  int maxIdx = 0;
  int maxVal = 0;
  for(int i = 0; i < numTSSP; i++){
    if(tsspTimesDetected[i] > maxVal){
      maxVal = tsspTimesDetected[i];
      maxIdx = i;
    }
  }
  return maxIdx;
}

bool IRSensor::isBallDetected(){
  for(int i = 0; i < numTSSP; i++){
    if(tsspTimesDetected[i] > 0) return true;
  }
  return false;
}

// Computes a weighted vector from the 3 front photodiodes, baseline-subtracted
// and clamped to their calibrated no-ball/max-ball range. Reuses the TSSP
// direction table since each photodiode is physically aligned with a TSSP index.
// Returns false (and leaves photoX/photoY untouched) if no photodiode sees
// a real signal above its noise margin.
bool IRSensor::calculatePhotoVector(float &photoX, float &photoY){
  float sumX = 0.0f, sumY = 0.0f;
  bool anyDetected = false;

  for(int i = 0; i < numPhotodiodes; i++){
    int adjusted = photodiodeReadings[i] - photoBaseline[i];
    if(adjusted < PHOTO_NOISE_MARGIN) continue; // no real detection on this sensor

    if(adjusted > photoMaxReading) adjusted = photoMaxReading; // clamp to calibrated max

    anyDetected = true;

    int dirIdx = photoDirIndex[i];
    float w = (float)adjusted * (float)adjusted; // square weighting, consistent with TSSP vector calc
    sumX += vectorX[dirIdx] * w;
    sumY += vectorY[dirIdx] * w;
  }

  if(!anyDetected || (sumX == 0.0f && sumY == 0.0f)) return false;

  photoX = sumX;
  photoY = sumY;
  return true;
}

// Weighted-vector angle from the peak sensor and its neighbors (uses instantaneous
// raw counts, not the slower distance-history smoothing, so angle stays responsive).
// Near the front (PHOTO_FRONT_CENTER_DEG +- PHOTO_WINDOW_HALF_DEG), blends in the
// higher-precision photodiode vector, weighted by how centered the ball is in that window.
void IRSensor::calculateBallVector(){
  int maxIndex = findMaxRawSensorIndex();
  int maxVal = tsspTimesDetected[maxIndex];

  pixels.clear();
  pixels.setPixelColor(maxIndex, pixels.Color(50, 50, 50));
  pixels.show();

  bool noVector = (maxVal == 0);
  float sumX = 0.0f, sumY = 0.0f;
  int reading = 0;

  if(!noVector){
    for(int d = -ANGLE_HALF_WINDOW; d <= ANGLE_HALF_WINDOW; d++){
      int idx = (maxIndex + d + numTSSP) % numTSSP;
      if(tsspTimesDetected[idx] < maxVal * 0.25f) continue; // ignore weak neighbors, same as peak-threshold filtering

      float w = (float)tsspTimesDetected[idx] * (float)tsspTimesDetected[idx]; // square weighting
      sumX += vectorX[idx] * w;
      sumY += vectorY[idx] * w;
      reading++;
    }

    if(reading == 0 || (sumX == 0.0f && sumY == 0.0f)){
      noVector = true;
    }
  }

  if(noVector){
    smoothAngle = 500;
    ballVectorX = 0;
    ballVectorY = 0;
    return;
  }

  // --- Photodiode front-refinement: hard switch with hysteresis ---
  // Use the last published smoothAngle to decide if we're in/near the front window.
  // This is deliberately based on the previous cycle's smoothed angle (not this cycle's raw
  // TSSP vector) since it's a stable, already-filtered estimate. The window itself widens by
  // PHOTO_WINDOW_HYSTERESIS once locked in, so a ball sitting right at the edge doesn't cause
  // rapid switching between TSSP and photodiode sources.
  if(smoothAngle != 500){
    float angleDiff = smoothAngle - PHOTO_FRONT_CENTER_DEG;
    // normalize to [-180, 180]
    while(angleDiff > 180.0f) angleDiff -= 360.0f;
    while(angleDiff < -180.0f) angleDiff += 360.0f;

    float activeHalfWindow = usingPhotoVector
      ? (PHOTO_WINDOW_HALF_DEG + PHOTO_WINDOW_HYSTERESIS)  // wider gate to STAY in
      : PHOTO_WINDOW_HALF_DEG;                              // normal gate to ENTER

    if(fabsf(angleDiff) <= activeHalfWindow){
      float photoX, photoY;
      if(calculatePhotoVector(photoX, photoY)){
        // Photodiodes only: fully replace the TSSP vector, no blending.
        sumX = photoX;
        sumY = photoY;
        usingPhotoVector = true;
      } else {
        // In the window but no real photodiode signal (e.g. ball just out of their FOV) -> fall back to TSSP
        usingPhotoVector = false;
      }
    } else {
      usingPhotoVector = false;
    }
  } else {
    usingPhotoVector = false;
  }

  if(!emaInit){
    filteredX = sumX;
    filteredY = sumY;
    emaInit = true;
  }

  // EMA smoothing applied to the vector components (avoids 0/360 wraparound issues)
  filteredX = (sumX * ANGLE_EMA_ALPHA) + (filteredX * (1.0f - ANGLE_EMA_ALPHA));
  filteredY = (sumY * ANGLE_EMA_ALPHA) + (filteredY * (1.0f - ANGLE_EMA_ALPHA));

  ballVectorX = filteredX;
  ballVectorY = filteredY;

  double theta = degrees(atan2(filteredY, filteredX));
  if(theta < 0) theta += 360.0;
  smoothAngle = (int)theta;
}

// Distance from the average of smoothed raw counts over a window around the peak sensor.
void IRSensor::calculateDistance(int maxIndex){
  uint32_t sum = 0;
  int count = 0;

  for(int d = -DIST_BEFORE; d <= DIST_AFTER; d++){
    int idx = (maxIndex + d + numTSSP) % numTSSP;
    sum += smoothedCount[idx];
    count++;
  }

  uint32_t avg = sum / count; // higher avg = stronger signal = closer ball

  // Map raw average hit-count to a 0-254 "distance" scale (0 = far/none, 254 = closest),
  // matching the convention used by getDistance() elsewhere in this codebase.
  // Tune maxExpectedDistance to your bufferSize/emitter power during calibration --
  // this was tuned against the old bufferSize=600/distHistCount=10 combo, so retest.
  const uint32_t maxExpectedDistance = 90;
  if(avg > maxExpectedDistance) avg = maxExpectedDistance;
  distance = maxExpectedDistance - avg; // invert so smaller number = closer, consistent with old convention
  filteredDistance = (distance * 0.1f) + (filteredDistance * (1.0f - 0.1f));
  //filteredDistance = avg;
}

void IRSensor::resetTracking(){
  smoothAngle = 500;
  ballVectorX = 0;
  ballVectorY = 0;
  filteredX = 0;
  filteredY = 0;
  emaInit = false;
  usingPhotoVector = false;

  distance = 254;
  filteredDistance = 254;

  histIndex = 0;
  histFull = false;
  memset(sensorHistory, 0, sizeof(sensorHistory));
  memset(smoothedCount, 0, sizeof(smoothedCount));

  pixels.clear();
  pixels.show();
}

int IRSensor::getAngle(){
  return smoothAngle;
}

int IRSensor::getDistance(){
  return (int)filteredDistance;
}

int IRSensor::getTSSPDetecting(){
  return tsspDetecting;
}

float IRSensor::getBallVectorX(){
  return ballVectorX;
}

float IRSensor::getBallVectorY(){
  return ballVectorY;
}

void IRSensor::printIR(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;

  if((millis() - lastUpdate) >= timeLimit){
    lastUpdate = millis();

    for(int i = 0; i < numTSSP; i++){
      Serial.print(smoothedCount[i]); Serial.print(' ');
    }
    Serial.println();

    for(int i = 0; i < numPhotodiodes; i++){
      Serial.print(photodiodeReadings[i] - photoBaseline[i]); Serial.print(' ');
    }
    Serial.println();

    Serial.print("smoothAngle: "); Serial.print(smoothAngle); Serial.print(' ');
    Serial.print("distance: "); Serial.print(filteredDistance); Serial.print('\n');
  }
}