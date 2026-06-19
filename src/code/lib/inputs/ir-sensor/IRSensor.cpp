/**
 * @file IRSensor.cpp
 * @brief Methods to get the angle and distance of the ball using vectors
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#include "IRSensor.h"

IRSensor::IRSensor() : pixels(numTSSP, neoPin, NEO_RGB + NEO_KHZ800){
  for (int i = 0; i < numTSSP; i++) {
    pinMode(tssp[i], INPUT);
  }

  for (int i = 0; i < numPhotodiodes; i++) {
    pinMode(photodiodes[i], INPUT);
  }
 
  for(int i = 0; i < numTSSP; i++){
    tsspTimesDetected[i] = 0;
  }

  for(int i = 0; i < numPhotodiodes; i++){
    photodiodeReadings[i] = 0;
  }

  analogReadResolution(12);
  analogReadAveraging(1);

  pixels.begin();
  pixels.clear();
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
  for(int i = 0; i < numTSSP; i++){
    bool currentDetection = !digitalReadFast(tssp[i]);
    tsspDetected[i][bufferIndex] = currentDetection;
  }

  bufferIndex++;
  if(bufferIndex >= bufferSize) bufferIndex = 0;

  for(int i = 0; i < numTSSP; i++){
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
    for(int i = 0; i < numPhotodiodes; i++){
      photodiodeReadings[i] = 0; //if no tssp detects anything, reset photodiodes to 0 to avoid noise
    }
  }
}

void IRSensor::updatePhotodiodes(){
  for(int i = 0; i < numPhotodiodes; i++){
    int currentDetection = max(analogRead(photodiodes[i]) - 215, 0);
    photodiodeReadings[i] = currentDetection;
  }
}

bool IRSensor::isBallDetected(){
  for(int i = 0; i < numTSSP; i++){
    if(tsspTimesDetected[i] > 0) return true;
  }
  return false;
}

bool IRSensor::arePhotodiodesDetecting(){
  for(int i = 0; i < numPhotodiodes; i++){
    if(photodiodeReadings[i] > 5) return true;
  }
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

  pixels.clear();

  if(isBallDetected()){
    // Find peak count
    int peakCount = 0;
    for(int i = 0; i < numTSSP; i++){
      if(tsspTimesDetected[i] > peakCount){
        peakCount = tsspTimesDetected[i];
        pixels.setPixelColor(i, pixels.Color(50, 50, 50)); // Set the color of the detected sensor to red
        pixels.show();
      }
      /* pixels.setPixelColor(i, pixels.Color(50, 50, 50)); // Set the color of the detected sensor to red
      pixels.show(); */
    }

    int threshold = peakCount * 0.8;  // Only consider sensors with at least 80% of the peak count

    for(int i = 0; i < numTSSP; i++){
      if(tsspTimesDetected[i] > threshold and threshold > 0){
        sumX += tsspTimesDetected[i] * vectorX[i];
        sumY += tsspTimesDetected[i] * vectorY[i];
        sensorsReading++;
      }
    }
  }
  
  tsspDetecting = (0.5 * sensorsReading) + ((1.0 - 0.5) * tsspDetecting);
  if(sensorsReading == 0){ rawAngle = 500; smoothAngle = 500; intensity = 0;}
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
      for(int i = 0; i < numPhotodiodes; i++){
        if(photodiodeReadings[i] > peakReading) peakReading = photodiodeReadings[i];
      }
      intensity = peakReading; //sqrt((filteredX * filteredX) + (filteredY * filteredY));
    }
  }
}

int IRSensor::getAngle(){
  return smoothAngle;
}

int IRSensor::getDistance(){
  if(intensity == 0) return 254;
  return map(200-intensity, 0, 200, 0, 253);
}

int IRSensor::getTSSPDetecting(){
  return tsspDetecting;
}

int IRSensor::getRelativeDistance(){
  int distance = getDistance();
  if(distance < 120) return 0;
  return 16 - tsspDetecting;
}

void IRSensor::printIR(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;
  
  if((millis() - lastUpdate) >= timeLimit){
    lastUpdate = millis();

    for(int i = 0; i < numTSSP; i++){
      //Serial.print(tsspTimesDetected[i]); Serial.print(' ');
    }
    //Serial.println();
    
    //Serial.print("rawAngle: "); Serial.print(rawAngle); Serial.print('\t');
    Serial.print("smoothAngle: "); Serial.print(smoothAngle); Serial.print(' ');
    //Serial.print("distance: "); Serial.print(getDistance()); Serial.print(' ');
    //Serial.print("intensity: "); Serial.print(intensity); Serial.println();
    //Serial.print("photodiode readings"); Serial.print(photodiodeReadings[0]); Serial.print(' ');
    //Serial.print(photodiodeReadings[1]); Serial.print(' ');
    //Serial.print(photodiodeReadings[2]); Serial.print(' ');
    Serial.print("TSSP Detecting: "); Serial.print(tsspDetecting); Serial.println();
    Serial.print("Relative Distance: "); Serial.print(getRelativeDistance()); Serial.println();
    //Serial.println();
  }
}