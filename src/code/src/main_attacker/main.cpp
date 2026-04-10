#include <Arduino.h>
#include "drive/Drive.h"
#include "imu/IMU.h"
#include "UART.h"
#include "pd-control/PD.h"

Drive drive;
IMU imu;
UART uart(Serial8, Serial5);
PD pd(4, 0.1, 200);

unsigned long long updateTimer = 0;

int yawCorrection = 0;

bool lineNeoOn = true;
const int lineNeoPin = 23;

bool firstDetected = false;
int firstSector = -1;

bool lineDetected(){
  if(uart.lineAngle*2 <= 360) return true;
  return false;
}

bool ballDetected(){
  if(uart.irAngle*2 <= 360) return true;
  return false;
}

int adjustBallAngleClose(int angle){
  if(angle > 360 || angle < 0){
    return 500;  // Invalid angle
  }
  
  // Right side: 271° to 79° (wrapping around 0°)
  if(angle > 270 || angle < 70){
    int adjusted = angle - 90;
    // Fix negative modulo
    if(adjusted < 0) adjusted += 360;
    return adjusted;
  }
  // Left side: 101° to 269°
  else if(angle > 110 && angle < 270){
    int adjusted = angle + 90;
    // Handle wrap-around
    if(adjusted >= 360) adjusted -= 360;
    return adjusted;
  }
  // Front: 80° to 100° - go straight
  else{
    return 90;
  }
}

int adjustLineAngle(int angle){
  if(angle >= 0 && angle < 180){
    return angle + 180;
  } else if(angle <= 360 && angle >= 180){
    return angle - 180;
  } else{
    return angle;
  }
}

int getLineSector(int lineAngle) {
  lineAngle = (lineAngle - 90 + 360) % 360;
  while(lineAngle < -15) lineAngle += 360;
  while(lineAngle >= 345) lineAngle -= 360;

  for(int i = 0; i < 12; i++) {
    int lower = -15 + (i * 30);
    int upper = 15 + (i * 30);
    if(lineAngle >= lower && lineAngle < upper) {
      return i;
    }
  }
  return -1;
}

int line_switch(int sector, int lastSector) {
  int angle = sector * 30;

  if(lastSector <= 3) {
    if(3 + lastSector <= sector && sector <= 8 + lastSector) {
      if(sector == 3) angle = 90;
      else angle = lastSector * 30;
    }
  } else if(4 <= lastSector && lastSector <= 8) {
    if(sector <= lastSector - 4 || lastSector + 3 <= sector) {
      angle = lastSector * 30;
    }
  } else if(9 <= lastSector) {
    if(lastSector - 9 <= sector && sector <= lastSector - 4) {
      angle = lastSector * 30;
    }
  }

  angle = (angle % 360 + 360) % 360;
  return (angle + 90) % 360;
}

int getAvoidLineAngle(){
  if(!firstDetected){
    firstSector = getLineSector(uart.lineAngle*2);
    firstDetected = true;
  }
  int sector = getLineSector(uart.lineAngle*2);
  int avoidAngle = adjustLineAngle(line_switch(sector, firstSector));
  return avoidAngle;
}

int getMovementAngle(){
  if(lineDetected()) return getAvoidLineAngle();
  if(uart.irClose == 1) return adjustBallAngleClose(uart.irAngle*2);
  return uart.irAngle*2;
}

int getPWM(){
  if(adjustBallAngleClose(uart.irAngle*2) == 90) return 210;
  if(uart.irClose == 1) return 150;
  return 200;
}

void setup() {
  Serial.begin(115200);
  uart.begin(2000000);
  if(lineNeoOn){
    pinMode(lineNeoPin, OUTPUT);
    digitalWrite(lineNeoPin, HIGH);
  }
  delay(1000);

  if (!imu.begin(Serial7)) {
    Serial.println("imu not found");
  }
}

void loop() {
  uart.receive();

  if(millis() - updateTimer >= 10){
    updateTimer = millis();
    if(imu.update()) yawCorrection = pd.getCorrection(imu.getYaw());
    Serial.print("IR Angle: ");Serial.println(uart.irAngle*2);
    Serial.print("IR Close: ");Serial.println(uart.irClose);
    Serial.print("Line Angle: ");Serial.println(uart.lineAngle*2);
  }

  if(ballDetected() or lineDetected()) drive.driveToAngle(getMovementAngle(), getPWM(), yawCorrection);
  else drive.writeAllMotorsOutput(yawCorrection);
}