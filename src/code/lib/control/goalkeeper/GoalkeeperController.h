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
#include "game-logic/LineLogic.h"

struct MovementCommand {
  int angle;        // 0-360 degrees
  int power;          // 0-250
};

class GoalkeeperController {
  public:
    GoalkeeperController();
    
    /**
     * Calculate movement command based on line and ball vectors
     * @param lineAngle: Angle of the line vector
     * @param irAngle: Angle of the IR sensor reading
     * @param irDistance: Distance from IR sensor
     * @return MovementCommand with angle and power
     */
    MovementCommand calculateMovement( //recieves params, returns movement command function
      int lineAngle,
      int irAngle, int irDistance
    );

    LineLogic line_logic; // Instance of LineLogic for line processing
    
    // Tuning parameters
    void setLineCoefficient(float k) { k_line = k; }
    void setPowerLimit(int limit) { power_limit = limit; }
    void setSmoothing(float alpha) { smoothing_alpha = alpha; }
    
    float getLineCoefficient() { return k_line; }
    
  private:
    // Tuning coefficients
    float k_line = 1.0;                    // How strongly to follow the line
    int power_limit = 160;                 // Max power to motors
    float smoothing_alpha = 0.15;          // Exponential smoothing factor
    int ball_far_threshold = 254;          // Distance above which ball is "far"
    
    // State for exponential smoothing
    float smoothed_result_x = 0;
    float smoothed_result_y = 0;
    
    /**
     * Determine which side of the line the ball is on
     * @param line_angle: Angle of the line vector (radians)
     * @param ball_angle: Angle of the ball vector (radians)
     * @return -1 for left side, 1 for right side
     */
    int determineBallSide(int lineAngle, int ballAngle);
    
    int angleDifference(int angle1, int angle2);

    float magnitude(float x, float y);
};

#endif
