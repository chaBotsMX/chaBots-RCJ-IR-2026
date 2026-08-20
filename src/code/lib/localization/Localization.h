/**
 * @file Localization.h
 * @brief Robot localization for rectangular field using available distance sensors
 * @author Alfonso De Anda / chaBotsMX
 * @date 05/29/26
 * @modification Added support for partial sensor setup (Right & Back only)
 */

#ifndef Localization_H
#define Localization_H

#include <Arduino.h>

// ============================================================================
// SENSOR AVAILABILITY CONFIGURATION
// ============================================================================
#define HAS_FRONT   false   // Currently: not connected
#define HAS_BACK    true    // Currently: connected
#define HAS_LEFT    false   // Currently: not connected
#define HAS_RIGHT   true    // Currently: connected
// ============================================================================

class Localization {
  public:
    Localization() {};

    void update(int _frontDistance, int _backDistance, int _leftDistance, int _rightDistance) {
      // Calculate position based on available sensors
      updateVerticalPosition(_frontDistance, _backDistance);
      updateHorizontalPosition(_leftDistance, _rightDistance);
    }

    int getX() { return x; }
    int getY() { return y; }

    const int fieldWidth = 158; // cm
    const int fieldHeight = 219; // cm

  private:
    int x = 254; // X position of the robot in cm (0,0) is the center of the field
    int y = 254; // Y position of the robot in cm (0,0) is the center of the field

    const int distanceTolerance = 30; // cm, acceptable error margin for distance readings

    /**
     * Calculate vertical position (Y) based on available front/back sensors
     * 
     * With both Front & Back:  Y = (Back - Front) / 2
     * With only Back:          Assume Front = fieldHeight - Back, then calculate
     * With only Front:         Assume Back = fieldHeight - Front, then calculate
     * With neither:            Invalid (y = 254)
     */
    void updateVerticalPosition(int _frontDistance, int _backDistance) {
      int frontDist = _frontDistance;
      int backDist = _backDistance;

      // If Front sensor is missing, calculate it from Back
      if (!HAS_FRONT && HAS_BACK) {
        frontDist = fieldHeight - backDist;
      }
      // If Back sensor is missing, calculate it from Front
      else if (!HAS_BACK && HAS_FRONT) {
        backDist = fieldHeight - frontDist;
      }
      // If both are missing, mark as invalid
      else if (!HAS_FRONT && !HAS_BACK) {
        y = 254;
        return;
      }

      // Now validate and calculate Y with the available/estimated distances
      if (isVerticalDistanceValid(frontDist, backDist)) {
        y = (backDist - frontDist) / 2;
      } else {
        y = 254; // Invalid vertical distance readings
      }
    }

    /**
     * Calculate horizontal position (X) based on available left/right sensors
     * 
     * With both Left & Right:  X = (Left - Right) / 2
     * With only Right:         Assume Left = fieldWidth - Right, then calculate
     * With only Left:          Assume Right = fieldWidth - Left, then calculate
     * With neither:            Invalid (x = 254)
     */
    void updateHorizontalPosition(int _leftDistance, int _rightDistance) {
      int leftDist = _leftDistance;
      int rightDist = _rightDistance;

      // If Left sensor is missing, calculate it from Right
      if (!HAS_LEFT && HAS_RIGHT) {
        leftDist = fieldWidth - rightDist;
      }
      // If Right sensor is missing, calculate it from Left
      else if (!HAS_RIGHT && HAS_LEFT) {
        rightDist = fieldWidth - leftDist;
      }
      // If both are missing, mark as invalid
      else if (!HAS_LEFT && !HAS_RIGHT) {
        x = 254;
        return;
      }

      // Now validate and calculate X with the available/estimated distances
      if (isHorizontalDistanceValid(leftDist, rightDist)) {
        x = (leftDist - rightDist) / 2;
      } else {
        x = 254; // Invalid horizontal distance readings
      }
    }

    bool isVerticalDistanceValid(int _frontDistance, int _backDistance) {
      return _frontDistance + _backDistance <= fieldHeight + distanceTolerance && 
             _frontDistance + _backDistance >= fieldHeight - distanceTolerance;
    }

    bool isHorizontalDistanceValid(int _leftDistance, int _rightDistance) {
      return _leftDistance + _rightDistance <= fieldWidth + distanceTolerance && 
             _leftDistance + _rightDistance >= fieldWidth - distanceTolerance;
    }
};

#endif