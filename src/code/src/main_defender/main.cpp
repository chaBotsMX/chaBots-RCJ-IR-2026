/**
 * @file main.cpp
 * 
 * @author LeoMc. / chaBotsMX
 * @date 10/02/26
 */

/* #include <Arduino.h>
#include "Robot.h"
#include "UART.h"

Robot robot;
UART uart (Serial8, Serial5, Serial1);

unsigned long long updateTimer = 0;

int yawCorrection = 0;

int irAngle = 500;
int irDistance = 254;
int lineAngle = 500;
int cameraAngle = 0;

void setup() {
  Serial.begin(115200);
  uart.beginIR(2000000);
  uart.beginLine(2000000);
  uart.beginCamera(115200);

  robot.setLineNeo(true);

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
  cameraAngle = uart.cameraAngle;

  if(millis() - updateTimer >= 10){
    updateTimer = millis();
    
    robot.updateButtons();
    if(robot.imu.update()) yawCorrection = robot.getYawCorrection(200, irAngle);
    
    Serial.print("IR Angle: ");Serial.println(irAngle);
    Serial.print("IR Distance: ");Serial.println(irDistance);
    Serial.print("Line Angle: ");Serial.println(lineAngle);
    Serial.print("Camera Angle: ");Serial.println(cameraAngle);
  }

  if(robot.wasButton2Pressed()){
    robot.updateGoalkeeperControl(irAngle, irDistance, lineAngle, cameraAngle, robot.imu.getYaw());
    robot.drive.driveToAngle(robot.gkCmd.angle, robot.gkCmd.power, yawCorrection);
  } else{
    robot.drive.writeAllMotorsOutput(0);
  }
} */

#include <Arduino.h>
#include "Robot.h"
#include "UART.h"

Robot robot;
UART uart (Serial8, Serial5, Serial1);

unsigned long long updateTimer = 0;

int yawCorrection = 0;

int irAngle = 500;
int irDistance = 254;
int lineAngle = 500;
int cameraAngle = 200;

float angularOffset = 0;

int calculateApproximatePower(int irAngle) {
    int angleFromFront = irAngle - 90;
        
    // normalize
    if(angleFromFront > 180) angleFromFront -= 360;
    if(angleFromFront < -180) angleFromFront += 360;
        
    int absOffset = abs(angleFromFront);

    int minPower = 30;   // Power when ball exactly at 90°
    int midPower = 45;   // Power when ball at 60° or 120°
    int maxPower = 200;  // Power when ball at sides
        
    int basePower;
    
    if(absOffset < 15) {
        basePower = minPower;
    }
    else if(absOffset < 30) {
        basePower = maxPower * 0.9;
    }
    else if(absOffset < 90) {
        basePower = maxPower;
    }
    else {
        basePower = midPower;
    }
    
    int finalPower = (int)(basePower);
    
    return constrain(finalPower, 0, maxPower);
}

void setup() {
  Serial.begin(115200);
  uart.beginIR(2000000);
  uart.beginLine(2000000);
  uart.beginCamera(115200);

  robot.setLineNeo(true);

  Serial.print("Logic Lipo Voltage: "); Serial.println(robot.getLogicLipoVoltage());
  Serial.print("Power Lipo Voltage: "); Serial.println(robot.getPowerLipoVoltage());

  delay(1000);

  if (!robot.imu.begin(Serial7)) {
    Serial.println("imu not found");
  }

  Serial.println("1");
}

void loop() {
  uart.receive();
  robot.kicker.update();

  irAngle = uart.irAngle*2;
  irDistance = uart.irDistance;
  lineAngle = uart.lineAngle*2;
  cameraAngle = uart.cameraAngle;

  if(millis() - updateTimer >= 10){
    updateTimer = millis();
    
    robot.updateButtons();
    
    Serial.print("IR Angle: ");Serial.println(irAngle);
    //Serial.print("IR Distance: ");Serial.println(irDistance);
    Serial.print("Line Angle: ");Serial.println(lineAngle);
    //Serial.print("Yaw: "); Serial.println(robot.imu.getYaw());
    Serial.print("Camera Angle: ");Serial.println(cameraAngle);
  }

  if(robot.imu.update()) yawCorrection = robot.getYawCorrection(200, irAngle, irDistance);

  if(robot.wasButton2Pressed()){
    robot.updateAttackerControl(irAngle, irDistance, lineAngle, cameraAngle, false);
    int angle;
    int power;

    if(irAngle > 360){
      angle = 0;
      power = 0;
    }

    else if(cameraAngle > 140){
      angle = robot.atkCmd.angle;
      power = 90;
    }

    else if(cameraAngle < 30){
      angle = 180;
      power = 60;
    }

    else if(cameraAngle > 110){
      angle = 0;
      power = 60;
    }

    else if(robot.attacker.isBallOnFront(irAngle)){
      angle = 90;
      power = 70;
    }

    else if(irDistance < 220){
      if(irAngle >= 90 and irAngle <= 270){angle = 180; power = calculateApproximatePower(irAngle);} //left
      else {angle = 0; power = calculateApproximatePower(irAngle);}; // right
    }

    else{
      angle = 0;
      power = 0;
    }

    robot.drive.driveToAngle(angle, power, yawCorrection);

    if(robot.hasBall(irAngle, irDistance)){
      robot.kicker.kick();
    }
  } else{
    robot.drive.writeAllMotorsOutput(0);
  }
}