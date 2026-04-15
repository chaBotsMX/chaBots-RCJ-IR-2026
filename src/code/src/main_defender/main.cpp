/**
 * @file main.cpp
 * 
 * @author LeoMc. / chaBotsMX
 * @date 10/02/26
 */

#include <Arduino.h>
#include "Robot.h"
#include "UART.h"

Robot robot;
UART uart (Serial8, Serial5);

unsigned long long updateTimer = 0;

int yawCorrection = 0;

int irAngle = 500;
int irDistance = 254;
int lineAngle = 500;

void setup() {
  Serial.begin(115200);
  uart.begin(2000000);
  
  Serial.print("Logic Lipo Voltage: "); Serial.println(robot.getLogicLipoVoltage());
  Serial.print("Power Lipo Voltage: "); Serial.println(robot.getPowerLipoVoltage());

  delay(1000);

  if (!robot.imu.begin(Serial7)) {
    Serial.println("imu not found");
  }
}

void loop() {
  uart.receive();
  //robot.kicker.update();

  irAngle = uart.irAngle*2;
  irDistance = uart.irDistance;
  lineAngle = uart.lineAngle*2;

  if(millis() - updateTimer >= 10){
    updateTimer = millis();
    
    if(robot.imu.update()) yawCorrection = robot.pd.getCorrection(robot.imu.getYaw());
    
    //Serial.print("IR Angle: ");Serial.println(irAngle);
    //Serial.print("IR Distance: ");Serial.println(irDistance);
    //Serial.print("Line Angle: ");Serial.println(lineAngle);
  }

  robot.updateGoalkeeperControl(irAngle, irDistance, lineAngle);

  robot.drive.driveToAngle(robot.gkCmd.angle, robot.gkCmd.power, yawCorrection);
}