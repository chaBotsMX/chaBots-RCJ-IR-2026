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
  int rotation;
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
      int currentLineAngle,
      int irAngle, int irDistance,
      int cameraAngle, int cameraDistance, float yaw
    );

    bool isBallOnFront(int irAngle, int irDistance);

    bool robotHasBall(int irAngle, int irDistance);

    float debug_distanceFactor;
    float debug_angleFactor;
    float debug_combinedFactor;

    LineAvoiding line;
    
  private:
    const int kIRDistanceOffset = 180; //
    const int kAvoidDistance = 170; //

    const int maxPower = 220;
    const int minPower = 80;

    const int apertureLeft = 80;
    const int apertureRight = 100;

    const float maxDistance = 100.0;

    const float kPowerP = 0.1;

    bool firstDetected = false;
    int initialLineAngle = 500;

    int getBallChasingAngle(int irAngle, int irDistance);
    int getBallChasingAngleNoDistance(int irAngle, int irDistance);
    int getBallChasingAngleNew(int irAngle, int irDistance);
    int getBallChasingPower(int irAngle, int irDistance);
};

#endif
