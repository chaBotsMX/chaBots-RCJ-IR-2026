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

        void update(int lineAngle){
            if(!recovering){
                if(lineDetected(lineAngle)){
                    recoveryAngle = (lineAngle + 180) % 360;
                    recoveryStartTime = millis();
                    recovering = true;
                }
            }

            if(recovering){
                if(millis() - recoveryStartTime >= 600) recovering = false;
            }
        }

        int getAvoidLineAngle(int lineAngle){
            update(lineAngle);
            if(recovering) return recoveryAngle;
            return 500;
        }

    private:
        int recoveryAngle = 0;
        uint32_t recoveryStartTime = 0;
        bool recovering = false;
};

#endif