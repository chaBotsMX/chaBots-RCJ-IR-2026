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
    consecutiveDetections[i] = 0;
    photodiodeReadings[i] = 1023;
  }
}

void IRSensor::update(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;
  
  if((micros() - lastUpdate) >= timeLimit){
    lastUpdate = micros();
    
    updateTSSP();
    calculateBallVector();
  }

  // Update photodiodes at SLOWER rate (not every loop!)
  static unsigned long lastPhotoUpdate = 0;
  if((millis() - lastPhotoUpdate) >= 5){  // Only every 10ms
    lastPhotoUpdate = millis();
    updatePhotodiodes();
  }
}

void IRSensor::updateTSSP(){  
  for(int i = 0; i < numSensors; i++){
    bool currentDetection = !digitalReadFast(tssp[i]);
    //Serial.print(currentDetection); Serial.print('\t');
    tsspDetected[i][bufferIndex] = currentDetection;

    if(currentDetection){
      consecutiveDetections[i]++;
    } else {
      consecutiveDetections[i] = 0;
    }
  }

  //Serial.println();

  bufferIndex++;
  if(bufferIndex >= bufferSize) bufferIndex = 0;

  for(int i = 0; i < numSensors; i++){
    int counter = 0;
    for(int j = 0; j < bufferSize; j++){
      if(tsspDetected[i][j]) counter++;
    }

/*     if(counter > 0 && consecutiveDetections[i] >= 0){
      tsspTimesDetected[i] = counter;
    } else {
      tsspTimesDetected[i] = 0;  //filter
    } */
    tsspTimesDetected[i] = counter;
  }
}

void IRSensor::updatePhotodiodes(){
  for(int i = 0; i < numSensors; i++){
    int currentDetection = analogRead(photodiodes[i]);
    photodiodeReadings[i] = currentDetection;
    //Serial.print(currentDetection); Serial.print('\t');
  }
  //Serial.println();
}

bool IRSensor::isBallDetected(){
  for(int i = 0; i < numSensors; i++){
    if(tsspTimesDetected[i] > 0 or photodiodeReadings[i] < 600) return true;
  }
  return false;
}

bool IRSensor::usingTSSP(){
  //todo: add logic for switching between tssp and photodiodes based on ball's distance
  return true;
}

void IRSensor::calculateBallVector(){
  float sumX = 0, sumY = 0;
  int sensorsReading = 0;

  if(usingTSSP()){
    for(int i = 0; i < numSensors; i++){
      if(tsspTimesDetected[i] > 0){
        sumX += tsspTimesDetected[i] * vectorX[i];
        sumY += tsspTimesDetected[i] * vectorY[i];
        sensorsReading++;
      }
    }
  }else{
    for(int i = 0; i < numSensors; i++){
      if(photodiodeReadings[i] < 900){
        sumX += 1023 - photodiodeReadings[i] * vectorX[i];
        sumY += 1023 - photodiodeReadings[i] * vectorY[i];
        sensorsReading++;
      }
    }
  }
  
  if(sensorsReading == 0) rawAngle = 500;
  else{
    double theta = degrees(atan2(sumY, sumX));
    if (theta < 0) theta+=360;
    rawAngle = (int)theta;

    if(usingTSSP()){
      magnitude = sqrt((sumX * sumX) + (sumY * sumY));
    }
  }
}

int IRSensor::getAngle(){
  return rawAngle;
}

int IRSensor::getDistance(){
  return magnitude;
}

void IRSensor::printIR(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;
  
  if((millis() - lastUpdate) >= timeLimit){
    lastUpdate = millis();

    //for(int i = 0; i < 100; i++){
      Serial.print("tssp: ");
      for(int j = 0; j < 16; j++){
        Serial.print(tsspTimesDetected[j]); Serial.print('\t');
        //Serial.print(photodiodeReadings[j]); Serial.print('\t');
      }
      //Serial.println();
    //}
    
    Serial.print("Angle: "); Serial.print(rawAngle); Serial.print('\t'); 
    Serial.print("Magnitude: "); Serial.print(magnitude); Serial.print('\n');
  }
}