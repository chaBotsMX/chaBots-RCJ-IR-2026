/**
 * @file Localization.h
 * @brief 
 * @author Alfonso De Anda / chaBotsMX
 * @date 05/29/26
 */

#ifndef Localization_H
#define Localization_H

#include <Arduino.h>
#include <distance-sensor/DistanceSensor.h>

class Localization {
  public:
    int frontDistance = 999, backDistance = 999, leftDistance = 999, rightDistance = 999;
    int x = 999, y = 999;

    DistanceSensor distanceSensor;
    Localization() : distanceSensor() {};

    std::pair<int, int> getBotPosition(){
      frontDistance = distanceSensor.getFrontDistance();
      backDistance = distanceSensor.getBackDistance();
      leftDistance = distanceSensor.getLeftDistance();
      rightDistance = distanceSensor.getRightDistance();

      if (isVerticalDistanceValid(frontDistance, backDistance)) y = (backDistance - frontDistance) / 2;
      else y = 999; // Invalid vertical distance readings

      if (isHorizontalDistanceValid(leftDistance, rightDistance)) x = (leftDistance - rightDistance) / 2;
      else x = 999; // Invalid horizontal distance readings

      return {x, y};
    }

  private:
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