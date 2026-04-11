/**
 * @file GoalkeeperController.h
 * @brief Vector-based goalkeeper control logic inspired by Edge RoboCup team
 * 
 * Implements the Shiokara vector sum algorithm:
 *   v_result = v_line + k * v_parallel
 * 
 * Where:
 * - v_line: Vector pointing back to the center of the white line (anchor)
 * - v_parallel: Vector perpendicular to line, direction based on ball position
 * - k: Coefficient controlling aggression vs stability
 * - v_result: Final movement vector to send to drive system
 * 
 * @author LeoMc. / chaBotsMX
 * @date 2026-03-30
 */

#ifndef GOALKEEPER_CONTROLLER_H //protect form other reads
#define GOALKEEPER_CONTROLLER_H // define if not 

#include <Arduino.h>
#include <math.h>

struct MovementCommand {
  float angle;        // 0-360 degrees
  int power;          // 0-250
  int rotation;       // -250 to 250 (yaw correction)
};

class GoalkeeperController {
  public:
    GoalkeeperController();
    
    /**
     * Calculate movement command based on line and ball vectors
     * @param line_x: X component of line vector
     * @param line_y: Y component of line vector
     * @param ball_x: X component of ball vector
     * @param ball_y: Y component of ball vector
     * @param yaw_correction: Correction from IMU yaw error (0 if not usied)
     * @return MovementCommand with angle and power
     */
    MovementCommand calculateMovement( //recieves params, returns movement command function
      float line_x, float line_y,
      float ball_x, float ball_y,
      int yaw_correction = 0 //tunable
    );
    
    // Tuning parameters
    void setLineCoefficient(float k) { k_line = k; }
    void setMinBallMagnitude(float mag) { min_ball_magnitude = mag; }
    void setPowerLimit(int limit) { power_limit = limit; }
    void setSmoothing(float alpha) { smoothing_alpha = alpha; }
    
    float getLineCoefficient() { return k_line; }
    float getMinBallMagnitude() { return min_ball_magnitude; }
    
  private:
    // Tuning coefficients
    float k_line = 1.0;                    // How strongly to follow the line
    float min_ball_magnitude = 0.5;        // Minimum ball detection to pursue
    int power_limit = 160;                 // Max power to motors dont remember how much was it haha
    float smoothing_alpha = 0.15;          // Exponential smoothing factor
    
    // State for exponential smoothing
    float smoothed_result_x = 0;
    float smoothed_result_y = 0;
    
    /**
     * Determine which side of the line the ball is on
     * @param line_angle: Angle of the line vector (radians)
     * @param ball_angle: Angle of the ball vector (radians)
     * @return 0 for left side, 1 for right side
     */
    int determineBallSide(float line_angle, float ball_angle);
    
    /**
     * Normalize angle to -PI to PI range
     */
    float normalizeAngle(float angle);
    
    /**
     * Calculate magnitude of a 2D vector
     */
    float magnitude(float x, float y);
};

#endif
