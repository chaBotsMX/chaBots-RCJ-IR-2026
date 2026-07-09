/**
 * @file Localization.h
 * @brief 
 * @author Alfonso De Anda / chaBotsMX
 * @date 05/29/26
 */

#ifndef Localization_H
#define Localization_H

#include <Arduino.h>

class Localization {
  public:
    Localization() {};

    void update(int _frontDistance, int _backDistance, int _leftDistance, int _rightDistance) {
      if (isVerticalDistanceValid(_frontDistance, _backDistance)) y = (_backDistance - _frontDistance) / 2;
      else y = 254; // Invalid vertical distance readings

      if (isHorizontalDistanceValid(_leftDistance, _rightDistance)) x = (_leftDistance - _rightDistance) / 2;
      else x = 254; // Invalid horizontal distance readings
    }

    int getX() { return x; }
    int getY() { return y; }

  private:
    int x = 254; // X position of the robot in cm (0,0) is the center of the field
    int y = 254; // Y position of the robot in cm (0,0) is the center of the field
    const int fieldWidth = 158; // cm
    const int fieldHeight = 219; // cm

    const int distanceTolerance = 30; // cm, acceptable error margin for distance readings

    bool isVerticalDistanceValid(int _frontDistance, int _backDistance) {
      return _frontDistance + _backDistance <= fieldHeight + distanceTolerance && _frontDistance + _backDistance >= fieldHeight - distanceTolerance; // Valid range for vertical distances
    }

    bool isHorizontalDistanceValid(int _leftDistance, int _rightDistance) {
      return _leftDistance + _rightDistance <= fieldWidth + distanceTolerance && _leftDistance + _rightDistance >= fieldWidth - distanceTolerance; // Valid range for horizontal distances
    }
};

#endif