#ifndef LINE_H
#define LINE_H

#include <Arduino.h>

class LineLogic {
    public:
        LineLogic(){}
        
        bool lineDetected(int lineAngle) {
            if(lineAngle <= 360) return true;
            return false;
        }

        int adjustLineAngle(int angle){
            if(angle >= 0 && angle < 180){
                return angle + 180;
            } else if(angle <= 360 && angle >= 180){
                return angle - 180;
            } else{
                return angle;
            }
        }

        int getLineSector(int lineAngle) {
            lineAngle = (lineAngle - 90 + 360) % 360;
            while(lineAngle < -15) lineAngle += 360;
            while(lineAngle >= 345) lineAngle -= 360;

            for(int i = 0; i < 12; i++) {
                int lower = -15 + (i * 30);
                int upper = 15 + (i * 30);
                if(lineAngle >= lower && lineAngle < upper) {
                return i;
                }
            }
            return -1;
        }

        int line_switch(int sector, int lastSector) {
            int angle = sector * 30;

            if(lastSector <= 3) {
                if(3 + lastSector <= sector && sector <= 8 + lastSector) {
                if(sector == 3) angle = 90;
                else angle = lastSector * 30;
                }
            } else if(4 <= lastSector && lastSector <= 8) {
                if(sector <= lastSector - 4 || lastSector + 3 <= sector) {
                angle = lastSector * 30;
                }
            } else if(9 <= lastSector) {
                if(lastSector - 9 <= sector && sector <= lastSector - 4) {
                angle = lastSector * 30;
                }
            }

            angle = (angle % 360 + 360) % 360;
            return (angle + 90) % 360;
        }

        int getAvoidLineAngle(int lineAngle){
            if(!firstDetected){
                firstSector = getLineSector(lineAngle);
                firstDetected = true;
            }
            int sector = getLineSector(lineAngle);
            int avoidAngle = adjustLineAngle(line_switch(sector, firstSector));
            return avoidAngle;
        }

    private:
        bool firstDetected = false;
        int firstSector = -1;
};

#endif