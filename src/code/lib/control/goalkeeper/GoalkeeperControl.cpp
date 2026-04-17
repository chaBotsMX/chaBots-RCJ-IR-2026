/**
 * @file GoalkeeperControl.cpp
 * @brief Implementation of vector-based goalkeeper control
 * 
 * Based on the Shiokara/Edge RoboCup algorithm:
 * - Calculates line vector from ring of line sensors
 * - Determines ball position relative to line
 * - Sums vectors to create smooth, continuous movement
 * 
 * @author LeoMc. / chaBotsMX
 * @date 2026-03-30
 */

#include "GoalkeeperControl.h"

GoalkeeperControl::GoalkeeperControl() {
  smoothed_result_x = 0;
  smoothed_result_y = 0;
}

MovementCommandGk GoalkeeperControl::calculateMovement(int lineAngle, int irAngle, int irDistance, int cameraAngle, float yaw) {
  MovementCommandGk cmd;

  if(cameraAngle > 140){
    cmd.angle = 270;
    cmd.power = 130;
    return cmd;
  }
  
  // If no line detected
  if (lineAngle >= 360) {
    int angle = map(cameraAngle, 0, 140, 340, 200);
    cmd.angle = angle;
    cmd.power = 120;
    return cmd;
  }

  if(irAngle >= 360){
    cmd.angle = 0;
    cmd.power = 0;
    return cmd;
  }
  
  // Convert angles to radians for vector math
  float line_angle_rad = radians(lineAngle);
  
  // Convert line to unit vector
  float line_x = cos(line_angle_rad);
  float line_y = sin(line_angle_rad);

  // Calculate the perpendicular (parallel to line) vector
  // The perpendicular points either left or right of the line
  int ball_side = determineBallSide(lineAngle, irAngle);
  //Serial.print("Ball side: "); Serial.println(ball_side);
  
  // Calculate the direction perpendicular to the line
  float parallel_angle;
  float absolute_parallel_angle;
  if(lineAngle >= 0  and lineAngle <= 180) parallel_angle = line_angle_rad - M_PI_2;
  else parallel_angle = line_angle_rad + M_PI_2;
/*   if (ball_side == -1) {
    parallel_angle = line_angle_rad + M_PI_2; // M_PI_2 is a standard constant for PI/2
  } else {
    parallel_angle = line_angle_rad - M_PI_2;
  } */

  absolute_parallel_angle = degrees(parallel_angle) + yaw;
  while(absolute_parallel_angle > 180) absolute_parallel_angle -= 360;
  while(absolute_parallel_angle < -180) absolute_parallel_angle += 360;

  int blocker;
  if(absolute_parallel_angle < -25 and ball_side == 1) blocker = 0;
  else if(absolute_parallel_angle > 25 and ball_side == -1) blocker = 0;
  else blocker = 1;

  float parallel_x = cos(parallel_angle) * ball_side * blocker; // Move in the direction of the ball
  float parallel_y = sin(parallel_angle) * ball_side * blocker;

  // Vector sum
  // v_result = v_line + k * v_parallel
  // The v_line component keeps robot anchored to the line
  // The v_parallel component moves robot side-to-side to block ball
  
  float result_x = line_x + (k_line * parallel_x);
  float result_y = line_y + (k_line * parallel_y);
  
  // Apply exponential smoothing to reduce jitter
  smoothed_result_x = (result_x * smoothing_alpha) + (smoothed_result_x * (1.0 - smoothing_alpha));
  smoothed_result_y = (result_y * smoothing_alpha) + (smoothed_result_y * (1.0 - smoothing_alpha));
  
  // Convert to angle and magnitude
  float result_angle = degrees(atan2(smoothed_result_y, smoothed_result_x)); //exact angle of movement vector
  float result_magnitude = magnitude(smoothed_result_x, smoothed_result_y); //magnitud of movement vector
  
  if (result_angle < 0) result_angle += 360;

  Serial.print("Result angle: "); Serial.print(result_angle);
  Serial.print(" Parallel angle: "); Serial.print(degrees(parallel_angle));
  Serial.print(" Absolute parallel angle "); Serial.println(absolute_parallel_angle);
  //Serial.print("Line Angle: "); Serial.println(lineAngle);

  power_limit = calculateApproximatePower(irAngle, result_angle);
  
  // Calculate power based on result magnitude
  int scaled_power = (int)(result_magnitude * power_limit);
  
  // Constrain power
  scaled_power = constrain(scaled_power, 0, power_limit);
  
  // Build command
  cmd.angle = result_angle;
  cmd.power = scaled_power;
  
  return cmd;
}

int GoalkeeperControl::determineBallSide(int line_angle, int ball_angle) {
/*   float line_angle_rad = radians(line_angle);
  float ball_angle_rad = radians(ball_angle);
  // Normalize angles to -PI to PI
  float normalized_line = normalizeAngle(line_angle_rad);
  float normalized_ball = normalizeAngle(ball_angle_rad);
  
  // Calculate the angular difference
  float diff = normalized_ball - normalized_line;
  diff = normalizeAngle(diff);
  
  // If diff is positive, ball is on the "left" side (go_flag = 0)
  // If diff is negative, ball is on the "right" side (go_flag = 1)
  if (diff > 0 && diff < M_PI) {
    return -1;  // Left
  } else {
    return 1;  // Right
  }
 */
  if(ball_angle <= 360){
    if(ball_angle >= 90 and ball_angle <= 270) return -1; // left
    return 1; // right
  } //return 0;
  return 0;
}

float GoalkeeperControl::normalizeAngle(float angle) {
  while (angle > M_PI) angle -= 2 * M_PI;
  while (angle < -M_PI) angle += 2 * M_PI;
  return angle;
}

float GoalkeeperControl::magnitude(float x, float y) {
  return sqrt((x * x) + (y * y));
}

int GoalkeeperControl::calculateApproximatePower(int irAngle, int lineAngle) {
    if(lineAngle > 200 and lineAngle < 340) return 30;
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

bool GoalkeeperControl::isRobotOnEdge(int parallel_angle){
  if(parallel_angle % 360 > -25 and parallel_angle % 360 < 25) return false;
  return true;
}