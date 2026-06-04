/**
 * @file AttackerControl.cpp
 * @brief Implementation of attacker control
 *
 * @author Alfonso De Anda / chaBotsMX
 * @date 2026-04-12
 */

#include "AttackerControl.h"

AttackerControl::AttackerControl() {}

MovementCommandAtk AttackerControl::calculateMovement(int lineAngle, int irAngle, int irDistance) {
    MovementCommandAtk cmd;
  
    if(lineAngle <= 360) {
        cmd.angle = line.getAvoidLineAngle(lineAngle);
        cmd.power = 100; // Line detected
    }
    else if(irAngle <= 360) {
        cmd.angle = getBallChasingAngle(irAngle, irDistance);
        cmd.power = getBallChasingPower(irDistance);
    }
    else {
        cmd.angle = 0;
        cmd.power = 0; // No ball detected
    }
  
    return cmd;
}

int AttackerControl::getBallChasingAngle(int irAngle, int irDistance) {
    if(isBallOnFront(irAngle)) return 0; // Ball is close, go straight
    int xBall = irDistance * cos(radians(irAngle));
    int yBall = irDistance * sin(radians(irAngle));

    double t = ((xBall * xBall) + (yBall * yBall - kIRDistanceOffset)) / ((xBall * xBall) + (yBall - kIRDistanceOffset) * (yBall - kIRDistanceOffset));
    t = max(0.0, min(1.0, t)); // Clamp t to [0, 1]
    int xClosest = xBall * t;
    int yClosest = yBall - kIRDistanceOffset * t;

    bool shouldOrbit = ((xClosest - xBall) * (xClosest - xBall) + (yClosest - yBall) * (yClosest - yBall)) < (kAvoidDistance * kAvoidDistance);

    if (shouldOrbit) {
        int offsetAngle = asin(kIRDistanceOffset / irDistance);
        if(irAngle > 270 or irAngle < 90) return irAngle - offsetAngle;
        return irAngle + offsetAngle;
    }
    return (int)degrees(atan2(yBall - kIRDistanceOffset, xBall));
}

int AttackerControl::getBallChasingPower(int irDistance) {
    if(isBallOnFront(irDistance)) return maxPower; // Ball is close, full power
    return max(minPower, min(maxPower, minPower + (int)(irDistance * kPowerP)));
}

bool AttackerControl::isBallOnFront(int irAngle) {
    return (irAngle >= 80 && irAngle <= 100);
}

bool AttackerControl::robotHasBall(int irAngle, int irDistance){
    if((irAngle >= 80 and irAngle <= 100) and irDistance < 140) return true;
    return false;
}