/**
 * @file AttackerControl.h
 * @brief 
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 2026-04-12
 */

#ifndef ATTACKER_CONTROL_H //protect form other reads
#define ATTACKER_CONTROL_H // define if not 

#include <Arduino.h>
#include "LineAvoiding.h"

struct MovementCommandAtk {
  int angle;        // 0-360 degrees
  int power;          // 0-250
  float offset;
};

class AttackerControl {
  public:
    AttackerControl();
    
    /**
     * Calculate movement command based on line and ball vectors
     * @param lineAngle: Angle of the line vector
     * @param irAngle: Angle of the IR sensor reading
     * @param irDistance: Distance from IR sensor
     * @return MovementCommand with angle and power
     */
    MovementCommandAtk calculateMovement( //recieves params, returns movement command function
      int lineAngle,
      int irAngle, int irDistance,
      int cameraAngle
    );

    float getAngularOffset(int cameraAngle);

    bool isBallOnFront(int irAngle);


    LineAvoiding line;
    
  private:
    bool ballDetected(int irAngle);
    int adjustBallAngleClose(int irAngle);
    bool isBallClose(int irDistance);
    int calculateOrbitPower(int irAngle, int irDistance);
};

#endif
