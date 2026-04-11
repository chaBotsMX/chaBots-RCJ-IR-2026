#include <Arduino.h>
#include "Robot.h"
#include "UART.h"

Robot robot;
UART uart (Serial8, Serial5);

unsigned long long updateTimer = 0;

int yawCorrection = 0;

int irAngle = 500;
int irClose = 0;
int lineAngle = 500;

int movementAngle = 0;
int pwm = 0;

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
  irClose = uart.irClose;
  lineAngle = uart.lineAngle*2;

  if(millis() - updateTimer >= 10){
    updateTimer = millis();
    
    if(robot.imu.update()) yawCorrection = robot.pd.getCorrection(robot.imu.getYaw());
    
    Serial.print("IR Angle: ");Serial.println(irAngle);
    Serial.print("IR Close: ");Serial.println(irClose);
    Serial.print("Line Angle: ");Serial.println(lineAngle);
    Serial.print("Avoid Line Angle: "); Serial.println(robot.lineLogic.getAvoidLineAngle(lineAngle));
  }

  movementAngle = robot.getMovementAngle(irAngle, irClose, lineAngle);
  pwm = robot.getPWM(irAngle, irClose, lineAngle);

  robot.drive.driveToAngle(movementAngle, pwm, yawCorrection);
}