/**
 * @file Drive.cpp
 * @brief Logic for omnidirectional movement
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#include "Drive.h"

Drive::Drive(){
  for (int i = 0; i < 4; i++) {
    pinMode(pwmFowardPins[i], OUTPUT);
    pinMode(pwmReversePins[i], OUTPUT);
    analogWriteFrequency(pwmFowardPins[i], 200000);
    analogWriteFrequency(pwmReversePins[i], 200000);
  }
}

void Drive::driveToAngle(int angle, int power, int rotation = 0){
  power = constrain(power, 0, 250);
  rotation = constrain(rotation, -250, 250);
  
  float angleRad = radians(angle + 45);

  float frPower =  power * cos(angleRad);  // Front-Right
  float flPower =  power * sin(angleRad);  // Front-Left  
  float brPower = -power * sin(angleRad);  // Back-Right
  float blPower = -power * cos(angleRad);  // Back-Left
  
  //rotation component
  frPower += rotation;
  flPower += rotation;
  brPower += rotation;
  blPower += rotation;
  
  //scaling
  float maxPower = max(
    max(abs(frPower), abs(flPower)),
    max(abs(brPower), abs(blPower))
  );
  
  if (maxPower > 250) {
    float scale = 250.0 / maxPower;
    frPower *= scale;
    flPower *= scale;
    brPower *= scale;
    blPower *= scale;
  }
  
  writeMotorOutput(fr, (int)frPower);
  writeMotorOutput(fl, (int)flPower);
  writeMotorOutput(br, (int)brPower);
  writeMotorOutput(bl, (int)blPower);
}

void Drive::writeMotorOutput(int motor, int power){
  power = constrain(power, -250, 250);

  if (abs(power) < minPWM) {
    power = 0;
  }

  if(power > 0){
    analogWrite(pwmFowardPins[motor], abs(power)); analogWrite(pwmReversePins[motor], 0);
  } else if(power < 0){
    analogWrite(pwmFowardPins[motor], 0); analogWrite(pwmReversePins[motor], abs(power));
  } else {
    analogWrite(pwmFowardPins[motor], 0); analogWrite(pwmReversePins[motor], 0);
  }
}

void Drive::writeAllMotorsOutput(int power){
  power = constrain(power, -250, 250);

  for(int i = 0; i < 4; i++){
    writeMotorOutput(i, power);
  }
}

void Drive::brake(){
  for(int i = 0; i < 4; i++){
    digitalWrite(pwmFowardPins[i], HIGH);
    digitalWrite(pwmReversePins[i], HIGH);
  }
}