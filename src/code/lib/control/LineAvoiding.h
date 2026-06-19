#ifndef LINE_AVOIDING_H
#define LINE_AVOIDING_H

#include <Arduino.h>

class LineAvoiding {
    public:
        LineAvoiding(){}
        
        bool lineDetected(int lineAngle) {
            if(lineAngle <= 360) return true;
            return false;
        }

        boolean isInverted = false;

        int getAvoidLineAngle(int currentLineAngle, int pastLineAngle){
            float currentX = cos(radians(currentLineAngle));
            float currentY = sin(radians(currentLineAngle));
            float pastX = cos(radians(pastLineAngle));
            float pastY = sin(radians(pastLineAngle));
            
            float dotProduct = (currentX * pastX) + (currentY * pastY);
            if(dotProduct < 0) isInverted = !isInverted;
            return isInverted ? (currentLineAngle + 180) % 360 : currentLineAngle;
        }

    private:
        int recoveryAngle = 0;
        uint32_t recoveryStartTime = 0;
        bool recovering = false;
};

#endif