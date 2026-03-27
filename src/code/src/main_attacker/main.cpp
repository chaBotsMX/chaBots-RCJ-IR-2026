#include <Arduino.h>
#include "drive/Drive.h"
#include "imu/IMU.h"
#include "UART.h"
#include "pd-control/PD.h"

Drive drive;
IMU imu;
UART uart(Serial8, Serial5);
PD pd(3.1666, 0.06, 160);

unsigned long long updateTimer = 0;

int yawCorrection = 0;

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

  drive.driveToAngle(uart.irAngle*2, 160, yawCorrection);
}