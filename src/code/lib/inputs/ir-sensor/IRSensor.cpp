/**
 * @file IRSensor.cpp
 * @brief Methods to get the angle and distance of the ball using vectors
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#include "IRSensor.h"

IRSensor::IRSensor(){
  for (int i = 0; i < numSensors; i++) {
    pinMode(tssp[i], INPUT);
    pinMode(photodiodes[i], INPUT);
  }
 
  for(int i = 0; i < numSensors; i++){
    tsspTimesDetected[i] = 0;
    photodiodeReadings[i] = 0;
  }

  analogReadResolution(12);
  analogReadAveraging(1);

  for(int i = 0; i < numSensors; i++){
    photodiodeGains[i] = referenceReadings[0] / referenceReadings[i];
  }
}

void IRSensor::update(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;
  
  if((micros() - lastUpdate) >= timeLimit){
    lastUpdate = micros();
    
    updateSensors();
    calculateBallVector();
  }
}

void IRSensor::updateSensors(){
  bool anyDetected = false;
  for(int i = 0; i < numSensors; i++){
    bool currentDetection = !digitalReadFast(tssp[i]);
    tsspDetected[i][bufferIndex] = currentDetection;
  }

  bufferIndex++;
  if(bufferIndex >= bufferSize) bufferIndex = 0;

  for(int i = 0; i < numSensors; i++){
    int counter = 0;
    for(int j = 0; j < bufferSize; j++){
      if(tsspDetected[i][j]) counter++;
    }

    tsspTimesDetected[i] = counter;
    if(tsspTimesDetected[i] < 10) tsspTimesDetected[i] = 0; //clean noise
    else anyDetected = true;
  }

  if(anyDetected) updatePhotodiodes(); //update photodiodes immediately if any tssp detects something
  else{
    for(int i = 0; i < numSensors; i++){
      photodiodeReadings[i] = 0; //if no tssp detects anything, reset photodiodes to 0 to avoid noise
    }
  }
}

void IRSensor::updatePhotodiodes(){
  for(int i = 0; i < numSensors; i++){
    int currentDetection = constrain(4070 - analogRead(photodiodes[i]), 0, 2000); //invert and cap to 0-2000
    photodiodeReadings[i] = currentDetection;
  }
}

bool IRSensor::isBallDetected(){
  for(int i = 0; i < numSensors; i++){
    if(tsspTimesDetected[i] > 0 or photodiodeReadings[i] < 600) return true;
  }
  return false;
}

bool IRSensor::arePhotodiodesDetecting(){
  int count = 0;
  for(int i = 0; i < numSensors; i++){
    if(photodiodeReadings[i] > 5) count++;
  }
  if(count > 2) return true;
  return false;
}

bool IRSensor::isBallClose(){
  if(getDistance() < 200) return true;
  return false;
}

void IRSensor::calculateBallVector(){
  float sumX = 0, sumY = 0;
  int sensorsReading = 0;
  int peakReading = 0;

  if(arePhotodiodesDetecting()){
    for(int i = 0; i < numSensors; i++){
      if(photodiodeReadings[i] > 5){
        sumX += photodiodeReadings[i] * vectorX[i];
        sumY += photodiodeReadings[i] * vectorY[i];
        sensorsReading++;
        if(photodiodeReadings[i] > peakReading) peakReading = photodiodeReadings[i];
      }
    }
  }else{
    // Find peak count
    int peakCount = 0;
    for(int i = 0; i < numSensors; i++){
      if(tsspTimesDetected[i] > peakCount){
        peakCount = tsspTimesDetected[i];
      }
    }

    int threshold = peakCount * 0.8;  // Only consider sensors with at least 80% of the peak count

    for(int i = 0; i < numSensors; i++){
      if(tsspTimesDetected[i] > threshold and threshold > 0){
        sumX += tsspTimesDetected[i] * vectorX[i];
        sumY += tsspTimesDetected[i] * vectorY[i];
        sensorsReading++;
      }
    }
  }
  
  if(sensorsReading == 0){ rawAngle = 500; smoothAngle = 500; intensity = 5000;}
  else{
    double theta = degrees(atan2(sumY, sumX));
    if (theta < 0) theta+=360;
    rawAngle = (int)theta;

    // Apply Linear Filter to the VECTORS
    filteredX = (sumX * filterAlpha) + (filteredX * (1.0 - filterAlpha));
    filteredY = (sumY * filterAlpha) + (filteredY * (1.0 - filterAlpha));

    // Calculate the Smooth Angle from the filtered vectors
    double smoothTheta = degrees(atan2(filteredY, filteredX));
    if (smoothTheta < 0) smoothTheta += 360;
    smoothAngle = (int)smoothTheta;

    if(arePhotodiodesDetecting()){
      intensity = peakReading; //sqrt((filteredX * filteredX) + (filteredY * filteredY));
    }
  }
}

int IRSensor::getAngle(){
  return smoothAngle;
}

int IRSensor::getDistance(){
  if(intensity <= 2000 && arePhotodiodesDetecting()) return map(2000 - intensity, 0, 2000, 0, 253);
  return 254;
}

void IRSensor::printIR(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;
  
  if((millis() - lastUpdate) >= timeLimit){
    lastUpdate = millis();

    for(int j = 0; j < 16; j++){
      //Serial.print(tsspTimesDetected[j]); Serial.print('\t');
      Serial.print(photodiodeReadings[j]); Serial.print(' ');
    }
    
    //Serial.print("rawAngle: "); Serial.print(rawAngle); Serial.print('\t');
    Serial.print("smoothAngle: "); Serial.print(smoothAngle); Serial.print(' ');
    Serial.print("isBallClose: "); Serial.print(isBallClose()); Serial.print(' ');
    Serial.print("arePhotodiodesDetecting: "); Serial.print(arePhotodiodesDetecting()); Serial.print(' ');
    Serial.print("distance: "); Serial.print(getDistance()); Serial.print('\n');
  }
}

float IRSensor::getBallVectorX(){
  return ballVectorX;
}

float IRSensor::getBallVectorY(){
  return ballVectorY;
}