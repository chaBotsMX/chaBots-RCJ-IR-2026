/**
 * @file LineSensor.cpp
 * @brief Methods to get the angle of the white line using vectors
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#include "LineSensor.h"

LineSensor::LineSensor() : pixels(numSensors, neoPin, NEO_RGB + NEO_KHZ800){
  for (int i = 0; i < numSensors; i++) {
    pinMode(comparators[i], INPUT);
  }
}

void LineSensor::begin(){
  analogWrite(vref[0], 200);
  analogWrite(vref[1], 200);
  analogWrite(vref[2], 200);
  analogWrite(vref[3], 200);

  pixels.begin();
  pixels.clear();

  for(int i = 0; i < numSensors; i++){
    unsigned long start = millis();
    while(millis() - start < 20){}
    pixels.setPixelColor(i, pixels.Color(250, 0, 0));
    pixels.show();
  }
}

void LineSensor::update(){
  for(int i = 0; i < numSensors; i++){
    readings[i] = digitalReadFast(comparators[i]);
  }
  calculateLineVector();
}

void LineSensor::calculateLineVector(){
  double tempSumX = 0;
  double tempSumY = 0;
  int sensorsReading = 0;

  for(int i = 0; i < numSensors; i++){
    if(readings[i]){
      sumX += vectorX[i];
      sumY += vectorY[i];
      sensorsReading++;
    }
  }

  if(sensorsReading == 0) {
    angle = 500;
    sumX = 0;
    sumY = 0;
  } else {
    sumX = tempSumX;
    sumY = tempSumY;
    double theta = degrees(atan2(sumY, sumX));
    if (theta < 0) theta+=360;
    angle = (int)theta;
  }
}

void LineSensor::printLS(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;

  if((millis() - lastUpdate) >= timeLimit){
    lastUpdate = millis();

    for(int i = 0; i< numSensors; i++){
      Serial.print(readings[i]); Serial.print(' ');
    }

    Serial.println(angle);
  }
}

int LineSensor::getAngle(){
  return angle/2;
}

float LineSensor::getLineVectorX(){
  return sumX;
}

float LineSensor::getLineVectorY(){
  return sumY;
}