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
    pinMode(pwmPins[i], OUTPUT);
    analogWriteFrequency(pwmPins[i], 20000);
    pinMode(in1Pins[i], OUTPUT);
    pinMode(in2Pins[i], OUTPUT);
  }
}

void Drive::driveToAngle(int angle, int power, int rotation){
  float brOutput = power * cos((-angle + 45) * PI / 180.0);
  float frOutput = -power * sin((-angle + 45) * PI / 180.0);
  float flOutput = power * sin((-angle + 45) * PI / 180.0);
  float blOutput = -power * cos((-angle + 45) * PI / 180.0);

  float maxVal = max(max(abs(brOutput), abs(frOutput)), max(abs(flOutput), abs(blOutput)));
  if (maxVal > 0) {
    scale = float(power) / maxVal;
    brOutput *= scale;
    frOutput *= scale;
    flOutput *= scale;
    blOutput *= scale;
  }

  writeMotorOutput(fr, frOutput + rotation);
  writeMotorOutput(br, brOutput + rotation);
  writeMotorOutput(bl, blOutput + rotation);
  writeMotorOutput(fl, flOutput + rotation);
}

void Drive::writeMotorOutput(int motor, int power){
  power = constrain(power, -255, 255);

  analogWrite(pwmPins[motor], abs(power));
  digitalWrite(in2Pins[motor], power > 0);
  digitalWrite(in1Pins[motor], power < 0);
}

void Drive::writeAllMotorsOutput(int power){
  power = constrain(power, -255, 255);

  for(int i = 0; i < 4; i++){
    analogWrite(pwmPins[i], abs(power));
    digitalWrite(in2Pins[i], power > 0);
    digitalWrite(in1Pins[i], power < 0);
  }
}