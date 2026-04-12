#ifndef BALL_H
#define BALL_H

#include <Arduino.h>

class BallLogic {
    public:
        BallLogic(){}
        
        bool ballDetected(int irAngle) {
            if(irAngle <= 360) return true;
            return false;
        }

        bool distanceClose(int irDistance){
            if(irDistance < 200) return true;
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
};

#endif