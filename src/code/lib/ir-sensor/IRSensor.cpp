/**
 * @file IRSensor.cpp
 * @brief Methods to get the angle and distance of the ball using vectors
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#include "IRSensor.h"

IRSensor::IRSensor(){
  for (int i = 0; i < numTSSP; i++) {
    pinMode(tssp[i], INPUT);
  }
}

void IRSensor::update(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;
  
  if((micros() - lastUpdate) >= timeLimit){
    lastUpdate = micros();
    
    updateTSSP();
    calculateBallVector();
  }
}

void IRSensor::updateTSSP(){  
  for(int i = 0; i < numTSSP; i++){
    bool currentDetection = !digitalReadFast(tssp[i]);
    tsspDetected[i][bufferIndex] = currentDetection;
    
    if(currentDetection){
      consecutiveDetections[i]++;
    } else {
      consecutiveDetections[i] = 0;
    }
  }

  bufferIndex++;
  if(bufferIndex >= bufferSize) bufferIndex = 0;

  for(int i = 0; i < numTSSP; i++){
    int counter = 0;
    for(int j = 0; j < bufferSize; j++){
      if(tsspDetected[i][j]) counter++;
    }
   tsspTimesDetected[i] = counter;
  }
}

void IRSensor::calculateBallVector(){
  float sumX = 0, sumY = 0;
  int sensorsReading = 0;

  for(int i = 0; i < numTSSP; i++){
    if(tsspTimesDetected[i] > 0){
      sumX += tsspTimesDetected[i] * vectorX[i];
      sumY += tsspTimesDetected[i] * vectorY[i];
      sensorsReading++;
    }
  }
  
  if(sensorsReading == 0) rawAngle = 500;
  else rawAngle = degrees(atan2(sumY, sumX)) + 180;
}

int IRSensor::getAngle(){
  return rawAngle;
}

void IRSensor::printIR(){
  Serial.print("Times detected: ");
  for(unsigned long t : tsspTimesDetected){
    Serial.print(t); Serial.print('\t');
  }
  
  Serial.print("Angle: "); Serial.print(rawAngle); Serial.print('\n');
}