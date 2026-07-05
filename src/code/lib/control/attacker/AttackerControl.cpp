/**
 * @file AttackerControl.cpp
 * @brief Implementation of attacker control
 *
 * @author Alfonso De Anda / chaBotsMX
 * @date 2026-04-12
 */

#include "AttackerControl.h"

AttackerControl::AttackerControl() {}

MovementCommandAtk AttackerControl::calculateMovement(int currentLineAngle, int irAngle, int irDistance, int cameraAngle, int cameraDistance, float yaw) {
    MovementCommandAtk cmd;
  
    if(line.lineDetected(currentLineAngle)) {
        if(!firstDetected) {
            initialLineAngle = currentLineAngle;
            firstDetected = true;
        }
        cmd.angle = line.getAvoidLineAngle(currentLineAngle, initialLineAngle);
        cmd.power = 100; // Line detected
        cmd.rotation = 0;
    }
    else if(irAngle <= 360) {
        if(cameraDistance < 100){
            int offset = cameraAngle - 70;
            cmd.rotation = yaw + offset;
        }
        else{cmd.rotation = 0;}
        //cmd.angle = getBallChasingAngleNoDistance(irAngle, irDistance);
        //cmd.angle = getBallChasingAngle(irAngle, irDistance);
        cmd.angle = getBallChasingAngleNew(irAngle, irDistance);
        cmd.power = getBallChasingPower(irAngle, irDistance);
        firstDetected = false; // Reset line detection when ball is detected
    }
    else {
        cmd.angle = 0;
        cmd.power = 0; // No ball detected
        cmd.rotation = 0;
        firstDetected = false; // Reset line detection when no ball is detected
    }
  
    return cmd;
}

int AttackerControl::getBallChasingAngle(int irAngle, int irDistance) {
    if(isBallOnFront(irAngle, irDistance)) return 90; // Ball is close, go straight
    int xBall = irDistance * cos(radians(irAngle));
    int yBall = irDistance * sin(radians(irAngle));

    double t = ((xBall * xBall) + (yBall * yBall - kIRDistanceOffset)) / ((xBall * xBall) + (yBall - kIRDistanceOffset) * (yBall - kIRDistanceOffset));
    t = max(0.0, min(1.0, t)); // Clamp t to [0, 1]
    int xClosest = xBall * t;
    int yClosest = (yBall - kIRDistanceOffset) * t;

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
    
    if(irDistance > 210) return irAngle;
    
    // Right side
    if(irAngle > 270 || irAngle < 75){
        int adjusted = irAngle - 90;
        // Fix negative modulo
        if(adjusted < 0) adjusted += 360;
        return adjusted;
    }
    // Left side
    else if(irAngle > 105 && irAngle < 270){
        int adjusted = irAngle + 90;
        // Handle wrap-around
        if(adjusted >= 360) adjusted -= 360;
        return adjusted;
    }
    else{
        return 90; // Ball is directly in front, go straight
    }
}

int AttackerControl::getBallChasingAngleNew(int irAngle, int irDistance) {
    float proximity = map(float(irDistance), 0.0, 254, 1.0, 0.0); // Closer ball gives higher proximity
    
    if(irAngle > 270 || irAngle < 75){ //right side
        int adjusted = irAngle - (90 * proximity); // Adjust angle based on proximity
        // Fix negative modulo
        if(adjusted < 0) adjusted += 360;
        return adjusted;
    }
    // Left side
    else if(irAngle > 105 && irAngle < 270){
        int adjusted = irAngle + (90 * proximity);
        // Handle wrap-around
        if(adjusted >= 360) adjusted -= 360;
        return adjusted;
    }
    else{
        return 90; // Ball is directly in front, go straight
    }
}

int AttackerControl::getBallChasingPower(int irAngle, int irDistance) {
    float distance = map(irDistance, 0, 254, 0.0, 1.0); // Closer ball gives higher proximity

    if(isBallOnFront(irAngle, irDistance)) return maxPower; // Ball is close, full power
    return (minPower + maxPower) / 2;
    //return max(minPower, min(maxPower, (maxPower) * distance));
}

bool AttackerControl::isBallOnFront(int irAngle, int irDistance) {
    if((irAngle >= 75 and irAngle <= 105) and irDistance < 240) return true;
    return false;
}

bool AttackerControl::robotHasBall(int irAngle, int irDistance){
    if((irAngle >= 80 and irAngle <= 100) and irDistance == 0) return true;
    return false;
}