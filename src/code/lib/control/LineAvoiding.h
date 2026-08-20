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
            float dotProduct = cos(radians(pastLineAngle - currentLineAngle));
            if(dotProduct < 0) return (pastLineAngle + 180) % 360; // if flipped
            return (currentLineAngle + 180) % 360;
        }

    private:
        int recoveryAngle = 0;
        uint32_t recoveryStartTime = 0;
        bool recovering = false;
};

#endif