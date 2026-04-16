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

MovementCommandGk GoalkeeperControl::calculateMovement(int lineAngle, int irAngle, int irDistance, int cameraAngle) {
  MovementCommandGk cmd;

  if(cameraAngle > 140){
    cmd.angle = 90;
    cmd.power = 60;
    return cmd;
  }
  
  // If no line detected
  if (lineAngle >= 360) {
    int angle = map(cameraAngle, 0, 140, 340, 200);
    cmd.angle = angle;
    cmd.power = 60;
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
  if(lineAngle >= 0  and lineAngle <= 180) parallel_angle = line_angle_rad - M_PI_2;
  else parallel_angle = line_angle_rad + M_PI_2;
/*   if (ball_side == -1) {
    parallel_angle = line_angle_rad + M_PI_2; // M_PI_2 is a standard constant for PI/2
  } else {
    parallel_angle = line_angle_rad - M_PI_2;
  } */
  float parallel_x = cos(parallel_angle) * ball_side; // Move in the direction of the ball
  float parallel_y = sin(parallel_angle) * ball_side;
  
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

  Serial.print("Result angle: "); Serial.print(result_angle); Serial.print(" Parallel angle: "); Serial.println(degrees(parallel_angle));
  //Serial.print("Line Angle: "); Serial.println(lineAngle);

  power_limit = calculateApproximatePower(irAngle);
  
  // Calculate power based on result magnitude
  int scaled_power = (int)(result_magnitude * power_limit);
  
  // Reduce power if ball is far away (conservative positioning)
  if (irDistance > ball_far_threshold) {
    scaled_power = (int)(scaled_power * 0.9);
  }
  
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

int GoalkeeperControl::calculateApproximatePower(int irAngle) {
    int angleFromFront = irAngle - 90;
        
    // normalize
    if(angleFromFront > 180) angleFromFront -= 360;
    if(angleFromFront < -180) angleFromFront += 360;
        
    int absOffset = abs(angleFromFront);

    int minPower = 20;   // Power when ball exactly at 90°
    int midPower = 120;   // Power when ball at 60° or 120°
    int maxPower = 140;  // Power when ball at sides
        
    int basePower;
    
    if(absOffset < 15) {
        basePower = minPower;
    }
    else if(absOffset < 40) {
        basePower = midPower;
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