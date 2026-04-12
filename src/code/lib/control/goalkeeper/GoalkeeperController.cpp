/**
 * @file GoalkeeperController.cpp
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

#include "GoalkeeperController.h"

GoalkeeperController::GoalkeeperController() {
  smoothed_result_x = 0;
  smoothed_result_y = 0;
}

MovementCommand GoalkeeperController::calculateMovement(int lineAngle, int irAngle, int irDistance) {
  MovementCommand cmd;
  
  // If no line detected, hold position (small movement to stay centered)
  if (line_logic.lineDetected(lineAngle)) {
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
  
  // Calculate the direction perpendicular to the line
  float parallel_angle = line_angle_rad + (M_PI / 2.0) ? ball_side == -1 : line_angle_rad - (M_PI / 2.0);

  float parallel_x = cos(parallel_angle);
  float parallel_y = sin(parallel_angle);
  
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
  
  // Calculate power based on result magnitude
  int scaled_power = (int)(result_magnitude * power_limit);
  
  // Reduce power if ball is far away (conservative positioning)
  if (irDistance > ball_far_threshold) {
    scaled_power = (int)(scaled_power * 0.6);
  }
  
  // Constrain power
  scaled_power = constrain(scaled_power, 0, power_limit);
  
  // Build command
  cmd.angle = result_angle;
  cmd.power = scaled_power;
  
  return cmd;
}

int GoalkeeperController::determineBallSide(int lineAngle, int ballAngle) {
  // Calculate shortest angular difference
  int diff = angleDifference(ballAngle, lineAngle);
  
  // If difference is positive (ball angle > line angle going CW),
  // ball is on the left side
  // If negative, ball is on the right side
  if (diff > 0 && diff <= 180) {
    return -1;  // Left side
  } else {
    return 1;   // Right side
  }
}

int GoalkeeperController::angleDifference(int angle1, int angle2) {
  // Calculate shortest angular distance from angle2 to angle1
  int diff = angle1 - angle2;
  
  // Normalize to -180 to +180
  while (diff > 180) diff -= 360;
  while (diff < -180) diff += 360;
  
  return diff;
}

float GoalkeeperController::magnitude(float x, float y) {
  return sqrt((x * x) + (y * y));
}
