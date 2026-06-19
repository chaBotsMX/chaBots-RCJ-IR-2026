/**
 * @file AttackerControl.cpp
 * @brief Implementation of attacker control
 *
 * @author Alfonso De Anda / chaBotsMX
 * @date 2026-04-12
 */

#include "AttackerControl.h"

AttackerControl::AttackerControl() {}

MovementCommandAtk AttackerControl::calculateMovement(int currentLineAngle, int pastLineAngle, int irAngle, int irDistance) {
    MovementCommandAtk cmd;
  
    if(line.lineDetected(currentLineAngle) && line.lineDetected(pastLineAngle)) {
        cmd.angle = line.getAvoidLineAngle(currentLineAngle, pastLineAngle);
        cmd.power = 100; // Line detected
    }
    else if(irAngle <= 360) {
        cmd.angle = getBallChasingAngleNoDistance(irAngle, irDistance);
        cmd.power = getBallChasingPower(irAngle, irDistance);
    }
    else {
        cmd.angle = 0;
        cmd.power = 0; // No ball detected
    }
  
    return cmd;
}

int AttackerControl::getBallChasingAngle(int irAngle, int irDistance) {
    if(isBallOnFront(irAngle, irDistance)) return 0; // Ball is close, go straight
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

int AttackerControl::getBallChasingAngleNoDistance(int irAngle, int irDistance) {
    if(irAngle > 360 || irAngle < 0){
        return 500;  // Invalid angle
    }
    
    if(isBallOnFront(irAngle, irDistance)) return 90; // Ball is close, go straight
    
    if(irDistance > 14) return irAngle;
    
    // Right side
    if(irAngle > 270 || irAngle < 80){
        int adjusted = irAngle - 90;
        // Fix negative modulo
        if(adjusted < 0) adjusted += 360;
        return adjusted;
    }
    // Left side
    else if(irAngle > 100 && irAngle < 270){
        int adjusted = irAngle + 90;
        // Handle wrap-around
        if(adjusted >= 360) adjusted -= 360;
        return adjusted;
    }
    else{
        return 90; // Ball is directly in front, go straight
    }
}

int AttackerControl::getBallChasingPower(int irAngle, int irDistance) {
    if(isBallOnFront(irAngle, irDistance)) return maxPower; // Ball is close, full power
    return (minPower + maxPower) / 2;
    //return max(minPower, min(maxPower, minPower + (int)(irDistance * kPowerP)));
}

bool AttackerControl::isBallOnFront(int irAngle, int irDistance) {
    if((irAngle >= 50 and irAngle <= 130) and irDistance == 0) return true;
    return false;
}

bool AttackerControl::robotHasBall(int irAngle, int irDistance){
    if((irAngle >= 80 and irAngle <= 100) and irDistance == 0) return true;
    return false;
}