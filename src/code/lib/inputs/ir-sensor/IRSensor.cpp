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
}

void IRSensor::update(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;
  
  if((micros() - lastUpdate) >= timeLimit){
    lastUpdate = micros();
    
    updateTSSP();
    updatePhotodiodes();
    calculateBallVector();
  }
}

void IRSensor::updateTSSP(){  
  for(int i = 0; i < numSensors; i++){
    bool currentDetection = digitalRead(tssp[i]);
    //Serial.print(currentDetection); Serial.print('\t');
    tsspDetected[i][bufferIndex] = currentDetection;
  }

  //Serial.println();

  bufferIndex++;
  if(bufferIndex >= bufferSize) bufferIndex = 0;

  for(int i = 0; i < numSensors; i++){
    int counter = 0;
    for(int j = 0; j < bufferSize; j++){
      if(tsspDetected[i][j]) counter++;
    }
   tsspTimesDetected[i] = counter;
  }
}

void IRSensor::updatePhotodiodes(){
  for(int i = 0; i < numSensors; i++){
    int currentDetection = analogRead(photodiodes[i]);
    Serial.print(currentDetection); Serial.print('\t');
  }
  Serial.println();
}

void IRSensor::calculateBallVector(){
  float sumX = 0, sumY = 0;
  int sensorsReading = 0;

  for(int i = 0; i < numSensors; i++){
    if(tsspTimesDetected[i] > 0){
      sumX += tsspTimesDetected[i] * vectorX[i];
      sumY += tsspTimesDetected[i] * vectorY[i];
      sensorsReading++;
    }
  }
  
  if(sensorsReading == 0) rawAngle = 500;
  else{
    double theta = degrees(atan2(sumY, sumX));
    if (theta < 0) theta+=360;
    rawAngle = (int)theta;
  }
}

int IRSensor::getAngle(){
  return rawAngle;
}

void IRSensor::printIR(){
  Serial.print("TSSP: ");
  for(int i = 0; i < 100; i++){
    for(int j = 0; j < 16; j++){
      Serial.print(tsspDetected[j][i]); Serial.print('\t');
    }
    Serial.println();
  }
  
  //Serial.print("Angle: "); Serial.print(rawAngle); Serial.print('\n');
}