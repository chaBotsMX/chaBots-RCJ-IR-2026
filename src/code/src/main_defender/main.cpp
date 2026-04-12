/**
 * @file main.cpp
 * 
 * @author LeoMc. / chaBotsMX
 * @date 10/02/26
 */
#include <Arduino.h>
#include "drive/Drive.h"
#include "imu/IMU.h"
#include "UART.h"
#include "pd-control/PD.h"
#include "goalkeeper/GoalkeeperController.h"

Drive drive; //motors
IMU imu; //orientation sensor
UART uart(Serial8, Serial5); // Serial8 for IMU, Serial5 for sensor board UART derial to serial
PD pd(1.85, 0.1, 160); // Proportional gain tuned for strong response to yaw error, with some integral to reduce steady-state error. Power limit set to prevent oscillation at high speeds.
GoalkeeperController goalkeeper;

unsigned long long updateTimer = 0;
int yawCorrection = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!imu.begin(Serial8)) {
    Serial.println("imu not found");
  }
  
  // Configure goalkeeper tuning parameters
  goalkeeper.setLineCoefficient(1.2);      // How strongly to follow line
  goalkeeper.setMinBallMagnitude(2.0);     // Minimum ball signal to pursue
  goalkeeper.setPowerLimit(160);           // Max motor power
  goalkeeper.setSmoothing(0.15);           // Smoothing factor
}

void loop() {
  uart.receive();

  if(millis() - updateTimer >= 10){
    updateTimer = millis();
    if(imu.update()) yawCorrection = pd.getCorrection(imu.getYaw());
  }

  // Convert received angles (0-360) to vectors
  // This is a temporary solution using angles from UART
  // TODO: Update sensor boards to send raw vectors via UART instead of angles
  
  float line_angle_rad = radians(uart.lineAngle);
  float line_x = cos(line_angle_rad);
  float line_y = sin(line_angle_rad);
  
  float ball_angle_rad = radians(uart.irAngle);
  float ball_magnitude = (uart.irDistance / 100.0) * 5.0;  // Scale distance to vector magnitude
  float ball_x = ball_magnitude * cos(ball_angle_rad);
  float ball_y = ball_magnitude * sin(ball_angle_rad);
  
  // Calculate movement using vector-based goalkeeper logic
  MovementCommand cmd = goalkeeper.calculateMovement(
    line_x, line_y,
    ball_x, ball_y,
    yawCorrection
  );
  
  drive.driveToAngle(cmd.angle, cmd.power, cmd.rotation);
}