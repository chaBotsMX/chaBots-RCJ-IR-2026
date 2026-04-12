/**
 * @file AttackerControl.cpp
 * @brief Implementation of attacker control
 *
 * @author Alfonso De Anda / chaBotsMX
 * @date 2026-04-12
 */

#include "AttackerControl.h"

AttackerControl::AttackerControl() {}

MovementCommand AttackerControl::calculateMovement(int lineAngle, int irAngle, int irDistance) {
    MovementCommand cmd;
  
    if(line.getAvoidLineAngle(lineAngle) <= 360) cmd.angle = line.getAvoidLineAngle(lineAngle);
    else if(isBallClose(irDistance)) cmd.angle = adjustBallAngleClose(irAngle);
    else cmd.angle = irAngle;

    if(line.lineDetected(lineAngle)) cmd.power = 100; //line detected
    else if(adjustBallAngleClose(irAngle) == irAngle) cmd.power = 160; // ball in front
    else if(isBallClose(irDistance)) cmd.power = 80 + irDistance * 0.2; // ball close but not in front
    else if(ballDetected(irAngle)) cmd.power = 150; // ball detected
    else cmd.power = 0; // no ball detected
  
    return cmd;
}

bool ballDetected(int irAngle) {
    if(irAngle <= 360) return true;
        return false;
    }

int adjustBallAngleClose(int irAngle){
    if(irAngle > 360 || irAngle < 0){
        return 500;  // Invalid angle
    }
            
    // Right side
    if(irAngle > 270 || irAngle < 70){
        int adjusted = irAngle - 90;
        // Fix negative modulo
        if(adjusted < 0) adjusted += 360;
        return adjusted;
    }
    // Left side
    else if(irAngle > 110 && irAngle < 270){
        int adjusted = irAngle + 90;
        // Handle wrap-around
        if(adjusted >= 360) adjusted -= 360;
        return adjusted;
    }
    // Front
    else{
        return irAngle;
    }
}

bool isBallClose(int irDistance){
    if(irDistance < 200) return true;
    return false;
}