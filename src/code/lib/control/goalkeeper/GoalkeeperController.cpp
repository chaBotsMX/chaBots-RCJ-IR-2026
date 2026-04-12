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

MovementCommand GoalkeeperController::calculateMovement( // calc belongs to Goalkeeper box and returns a movement command function
  float line_x, float line_y,
  float ball_x, float ball_y,
  int yaw_correction
) {
  MovementCommand cmd;
  cmd.rotation = yaw_correction;
  
  // If no line detected, hold position (small movement to stay centered)
  if (magnitude(line_x, line_y) < 0.1) {
    cmd.angle = 0;
    cmd.power = 0;
    return cmd;
  }
  
  //Get line angle and normalize
  float line_angle = atan2(line_y, line_x);  // In radians
  
  // Determine which side of the line the ball is on
  // We need to check if ball is in the forward-facing semicircle
  float ball_angle = atan2(ball_y, ball_x);  // In radians
  float ball_magnitude = magnitude(ball_x, ball_y);
  
  // Calculate the perpendicular (parallel to line) vector
  // The perpendicular points either left or right of the line
  int ball_side = determineBallSide(line_angle, ball_angle);
  
  // Calculate the direction perpendicular to the line
  float parallel_angle = line_angle + (M_PI / 2.0);
  if (ball_side == 1) {
    parallel_angle = line_angle - (M_PI / 2.0);
  }
  
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
  float result_angle = atan2(smoothed_result_y, smoothed_result_x); //exact angle of movement vector
  float result_magnitude = magnitude(smoothed_result_x, smoothed_result_y); //magnitud of movement vector
  
  // Apply power scaling
  // If ball is close enough, use full power; otherwise scale back
  int scaled_power = (int)(result_magnitude * power_limit); //its an intager because power is an intager, and we need to convert from float to int
  
  if (ball_magnitude < min_ball_magnitude) {
    scaled_power = (int)(scaled_power * 0.7);  // Reduce power if ball far away
  }
  
  scaled_power = constrain(scaled_power, 0, power_limit); //if power is above limit, set to limit. if below 0, set to 0
  
  // Convert angle from radians (-PI to PI) to degrees (0-360) to invalidate negative angles
  cmd.angle = (int)degrees(result_angle);
  if (cmd.angle < 0) cmd.angle += 360;
  
  cmd.power = scaled_power;
  
  return cmd;
}

int GoalkeeperController::determineBallSide(float line_angle, float ball_angle) {
  // Normalize angles to -PI to PI
  float normalized_line = normalizeAngle(line_angle);
  float normalized_ball = normalizeAngle(ball_angle);
  
  // Calculate the angular difference
  float diff = normalized_ball - normalized_line;
  diff = normalizeAngle(diff);
  
  // If diff is positive, ball is on the "left" side (go_flag = 0)
  // If diff is negative, ball is on the "right" side (go_flag = 1)
  if (diff > 0 && diff < M_PI) {
    return 0;  // Left
  } else {
    return 1;  // Right
  }
}

float GoalkeeperController::normalizeAngle(float angle) {
  while (angle > M_PI) angle -= 2 * M_PI;
  while (angle < -M_PI) angle += 2 * M_PI;
  return angle;
}

float GoalkeeperController::magnitude(float x, float y) {
  return sqrt((x * x) + (y * y));
}
