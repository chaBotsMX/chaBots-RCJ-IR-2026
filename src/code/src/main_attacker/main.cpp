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

bool ballDetected(){
  if(uart.irAngle*2 <= 360) return true;
  return false;
}

int adjustBallAngleClose(int angle){
  if(angle > 360 || angle < 0){
    return 500;  // Invalid angle
  }
  
  // Right side: 271° to 79° (wrapping around 0°)
  if(angle > 270 || angle < 65){
    int adjusted = angle - 90;
    // Fix negative modulo
    if(adjusted < 0) adjusted += 360;
    return adjusted;
  }
  // Left side: 101° to 269°
  else if(angle > 115 && angle < 270){
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

int getMovementAngle(){
  if(uart.irClose == 1) return adjustBallAngleClose(uart.irAngle*2);
  return uart.irAngle*2;
}

void setup() {
  Serial.begin(115200);
  uart.begin(1000000);
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
    Serial.println(imu.getYaw());
    Serial.print("IR Angle: ");Serial.println(uart.irAngle*2);
  }

  if(ballDetected()) drive.driveToAngle(getMovementAngle(), 180, yawCorrection);
  else drive.writeAllMotorsOutput(yawCorrection);
}