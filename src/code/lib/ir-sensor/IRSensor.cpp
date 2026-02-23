/**
 * @file IRSensor.cpp
 * @brief Methods to get the angle and distance of the ball using vectors
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#include "IRSensor.h"

int tsspTimesDetecting[numIR];

IRSensor::IRSensor(){
  for (int i = 0; i < numIR; i++) {
    pinMode(ir[i], INPUT);
  }
}

void IRSensor::update(unsigned long timeLimit){
  static int updateCounter = 0;

  unsigned long lastUpdate = 0;

  if((micros() - lastUpdate) > timeLimit){
    lastUpdate = micros();

    updateTSSP();
    calculateBallVector();
    updateCounter++;

    if(updateCounter > 10){
      updateCounter = 0;
      for(int tssp = 0; tssp < numIR; tssp++){
        tsspTimesDetecting[tssp] = 0;
      }
    }
  }
}

void IRSensor::updateTSSP(){
  unsigned long currentMicros = micros();
  unsigned long currentMillis = millis();

  for(int i = 0; i < numIR; i++){
    detected[i] = !digitalReadFast(ir[i]); //get current readings as booleans

    if(detected[i]){
      tsspTimesDetecting[i]++; //add for each reading
    }
  }
}

void IRSensor::calculateBallVector(){
  float sumX = 0, sumY = 0;
  int sensorsReading = 0;

  for(int i = 0; i < numIR; i++){
    if(tsspTimesDetecting[i] > 0){
      sumX += tsspTimesDetecting[i] * vectorX[i];
      sumY += tsspTimesDetecting[i] * vectorY[i];
      sensorsReading++;
    }
  }
  
  if(sensorsReading == 0) rawAngle = 500;
  else rawAngle = (atan2(sumY, sumX) * (180.0 / M_PI)) + 180;

  //intensity = sqrt((sumX * sumX) + (sumY * sumY));
  //if(intensity > maxIntensity) intensity = maxIntensity;
}

int IRSensor::getAngle(){
  return rawAngle/2;
}

/* int IRSensor::getIntensity(){
  return map(intensity, 0, 2100, 0, 254);
} */

void IRSensor::printIR(int angle, int intensity, unsigned long timeLimit, bool all=false){
  unsigned long printUpdate = 0;
  if((millis() - printUpdate) > timeLimit){
    printUpdate = millis();

    if(all){
      for(unsigned long d : detected){
        Serial.print(d); Serial.print('\t');
      }
    }
    
    Serial.print(angle); Serial.print('\n');
  }
}