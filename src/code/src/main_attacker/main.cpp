#include <Arduino.h>
#include "Robot.h"
#include "UART.h"

Robot robot;
UART uart (Serial8, Serial5, Serial2);

unsigned long long updateTimer = 0;

int yawCorrection = 0;

int irAngle = 500;
int irDistance = 254;
int lineAngle = 500;
int cameraAngle = 200;

float angularOffset = 0;

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

  if(robot.imu.update()) yawCorrection = robot.getYawCorrection(cameraAngle, irAngle, irDistance);

  if(robot.wasButton2Pressed()){
    robot.updateAttackerControl(irAngle, irDistance, lineAngle, cameraAngle, false);

    robot.drive.driveToAngle(robot.atkCmd.angle, robot.atkCmd.power, yawCorrection);

    if(robot.hasBall(irAngle, irDistance)){
      robot.kicker.kick();
    }
  } else{
    robot.drive.writeAllMotorsOutput(0);
  }
}