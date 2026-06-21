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

  analogReadResolution(14);
  analogReadAveraging(16);

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
    int currentDetection = /* analogRead(photodiodes[i]);  */ min(max(analogRead(photodiodes[i]) - minPhotodiodeReading, 0), maxPhotodiodeReading);
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
    if(photodiodeReadings[i] > 10) return true;
  }
  return false;
}

bool IRSensor::isBallClose(){
  if(getDistance() < 200) return true;
  return false;
}

void IRSensor::calculateBallVector(){
  float tsspVectorSumX = 0, tsspVectorSumY = 0;
  float photoVectorX = 0, photoVectorY = 0;
  int tsspReading = 0;
  int photoReading = 0;

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
        tsspVectorSumX += tsspTimesDetected[i] * vectorX[i];
        tsspVectorSumY += tsspTimesDetected[i] * vectorY[i];
        tsspReading++;
      }
    }

    for(int i = 0; i < numPhotodiodes; i++){
      if(photodiodeReadings[i] > 10){ // Only consider photodiodes with a reading above the minimum to reduce noise
        photoVectorX += photodiodeReadings[i] * vectorX[i];
        photoVectorY += photodiodeReadings[i] * vectorY[i];
        photoReading++;
      }
    }

    tsspDetecting = (0.5 * tsspReading) + ((1.0 - 0.5) * tsspDetecting);
    if(tsspReading == 0){ rawAngle = 500; smoothAngle = 500; }
    else{
      double theta = degrees(atan2(tsspVectorSumY, tsspVectorSumX));
      if (theta < 0) theta+=360;
      rawAngle = (int)theta;

      // Apply Linear Filter to the VECTORS
      filteredX = (tsspVectorSumX * filterAlpha) + (filteredX * (1.0 - filterAlpha));
      filteredY = (tsspVectorSumY * filterAlpha) + (filteredY * (1.0 - filterAlpha));

      // Calculate the Smooth Angle from the filtered vectors
      double smoothTheta = degrees(atan2(filteredY, filteredX));
      if (smoothTheta < 0) smoothTheta += 360;
      smoothAngle = (int)smoothTheta;
    }

    if(photoReading == 0){intensity = 0; distance = 254; filteredDistance = 254;}
    else{
      distance = sqrt((photoVectorX * photoVectorX) + (photoVectorY * photoVectorY));
      //distance = min(max(distance, 0.0), 400.0);
      distance = map(distance, 0, 400, 0, 254);
      distance = 254 - distance; //invert distance so that higher values mean closer distance
      filteredDistance = max((distance * 0.05) + (filteredDistance * (1.0 - 0.05)), 0);
    }
  } else{
    rawAngle = 500;
    smoothAngle = 500;
    intensity = 0;
    distance = 254;
    filteredDistance = 254;
  }
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

int IRSensor::getRelativeDistance(){
  int distance = getDistance();
  if(distance < 160) return 0;
  return 16 - tsspDetecting;
}

void IRSensor::printIR(unsigned long timeLimit){
  static unsigned long lastUpdate = 0;
  
  if((millis() - lastUpdate) >= timeLimit){
    lastUpdate = millis();

    for(int i = 0; i < numTSSP; i++){
      Serial.print(photodiodeReadings[i]); Serial.print(' ');
    }
    Serial.println();
    
    //Serial.print("rawAngle: "); Serial.print(rawAngle); Serial.print('\t');
    Serial.print("smoothAngle: "); Serial.print(smoothAngle); Serial.print(' ');
    Serial.print("distance: "); Serial.print(getDistance()); Serial.print('\n');
    //Serial.print("intensity: "); Serial.print(intensity); Serial.println();
    //Serial.print("photodiode readings"); Serial.print(photodiodeReadings[0]); Serial.print(' ');
    //Serial.print(photodiodeReadings[1]); Serial.print(' ');
    //Serial.print(photodiodeReadings[2]); Serial.print(' ');
    //Serial.print("TSSP Detecting: "); Serial.print(tsspDetecting); Serial.println();
    //Serial.print("Relative Distance: "); Serial.print(getRelativeDistance()); Serial.println();
    //Serial.println();
  }
}