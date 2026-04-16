/**
 * @file AttackerControl.cpp
 * @brief Implementation of attacker control
 *
 * @author Alfonso De Anda / chaBotsMX
 * @date 2026-04-12
 */

#include "AttackerControl.h"

AttackerControl::AttackerControl() {}

MovementCommandAtk AttackerControl::calculateMovement(int lineAngle, int irAngle, int irDistance, int cameraAngle) {
    MovementCommandAtk cmd;
  
    if(lineAngle <= 360) {
        cmd.angle = line.getAvoidLineAngle(lineAngle);
        cmd.power = 140; // Line detected
        cmd.offset = 0;
    }
    else if(isBallOnFront(irAngle)) {
        cmd.angle = irAngle;
        cmd.power = 190; // Ball in front
        cmd.offset = getAngularOffset(cameraAngle);
    }
    else if(isBallClose(irDistance)) {
        cmd.angle = adjustBallAngleClose(irAngle);        
        cmd.power = calculateOrbitPower(irAngle, irDistance);
        cmd.offset = 0;
    }
    else if(ballDetected(irAngle)) {
        cmd.angle = irAngle;
        cmd.power = 120 + irDistance * 0.1; // Ball detected
        cmd.offset = 0;
    }
    else {
        cmd.angle = 0;
        cmd.power = 0; // No ball detected
        cmd.offset = 0;
    }
  
    return cmd;
}

bool AttackerControl::ballDetected(int irAngle) {
    if(irAngle <= 360) return true;
    return false;
}

int AttackerControl::adjustBallAngleClose(int irAngle){
    if(irAngle > 360 || irAngle < 0){
        return 500;  // Invalid angle
    }
            
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
    // Front
    else{
        return 90;
    }
}

bool AttackerControl::isBallOnFront(int irAngle){
    if(irAngle >= 75 and irAngle <= 105) return true;
    return false;
}

bool AttackerControl::isBallClose(int irDistance){
    if(irDistance < 230) return true;
    return false;
}

int AttackerControl::calculateOrbitPower(int irAngle, int irDistance) {
    int angleFromFront = irAngle - 90;
        
    // normalize
    if(angleFromFront > 180) angleFromFront -= 360;
    if(angleFromFront < -180) angleFromFront += 360;
        
    int absOffset = abs(angleFromFront);

    int minPower = 80;   // Power when ball exactly at 90°
    int midPower = 140;  // Power when ball at sides
    int maxPower = 180;  // Power when ball behind
        
    int basePower;
    
    if(absOffset < 30) {
        basePower = minPower;
    }
    else if(absOffset < 90) {
        basePower = midPower;
    }
    else {
        basePower = maxPower;
    }
    
    int finalPower = (int)(basePower);
    
    return constrain(finalPower, minPower, maxPower);
}

float AttackerControl::getAngularOffset(int cameraAngle){
    if(cameraAngle <= 140){
        return cameraAngle - 70;
    }
    return 0;
}