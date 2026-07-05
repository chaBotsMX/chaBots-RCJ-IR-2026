#include <Arduino.h>
#include "Robot.h"
#include "UART.h"

Robot robot;
UART uart (Serial8, Serial5, Serial2);
AttackerControl attacker;
MovementCommandAtk atkCmd;

unsigned long long updateTimer = 0;

int yawCorrection = 0;

int irAngle = 500;
int irDistance = 254;
int lineAngle = 500;
int pastLineAngle = 500;
int cameraAngle = 254;
int cameraDistance = 254;
int cameraConfidence = 254;

float angularOffset = 0;

void setup() {
  Serial.begin(115200);
  uart.beginIR(2000000);
  uart.beginLine(2000000);
  uart.beginCamera(115200);

  robot.setLineNeo(true);
  robot.display.begin();

  Serial.print("Logic Lipo Voltage: "); Serial.println(robot.getLogicLipoVoltage());
  Serial.print("Power Lipo Voltage: "); Serial.println(robot.getPowerLipoVoltage());

  if (!robot.imu.begin(Serial4)) {
    Serial.println("imu not found");
  }

  robot.display.showImage();

}

void loop() {
  uart.receiveIR();
  uart.receiveLine();
  uart.receiveCamera();
  robot.kicker.update();

  irAngle = uart.irAngle*2;
  irDistance = uart.irDistance;
  lineAngle = uart.lineAngle*2;
  cameraAngle = uart.cameraAngle;
  cameraDistance = uart.cameraDistance;
  cameraConfidence = uart.cameraConfidence;

  if(millis() - updateTimer >= 10){
    updateTimer = millis();
    
    robot.updateButtons();
    
    //Serial.print("IR Angle: ");Serial.println(irAngle);
    //Serial.print("IR Distance: ");Serial.println(irDistance);
    //Serial.print("Line Angle: ");Serial.println(lineAngle);
    //Serial.print("Camera Angle: ");Serial.println(cameraAngle);
    //Serial.print("Camera Distance: ");Serial.println(cameraDistance);
    //Serial.print("Camera Confidence: ");Serial.println(cameraConfidence);
    //Serial.print("Yaw: "); Serial.println(robot.imu.getYaw());
    //Serial.print("Camera Angle: ");Serial.println(cameraAngle);
    Serial.println(map(float(irDistance), 0.0, 254, 1.0, 0.0));
  }

  if(robot.wasButton2Pressed()){
    atkCmd = attacker.calculateMovement(lineAngle, irAngle, irDistance, cameraAngle, cameraDistance, robot.imu.getYaw());

    if(robot.imu.update()) yawCorrection = robot.getYawCorrection(atkCmd.rotation);

    robot.drive.driveToAngle(atkCmd.angle, atkCmd.power, yawCorrection);

    //if(robot.hasBall(irAngle, irDistance)){
      //robot.kicker.kick();
    //}
  } else{
    robot.drive.writeAllMotorsOutput(0);
  }
}