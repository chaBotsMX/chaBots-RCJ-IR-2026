#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include "drive/Drive.h"
#include "kicker/Kicker.h"
#include "imu/IMU.h"
#include "pd-control/PD.h"
#include "game-logic/BallLogic.h"
#include "game-logic/LineLogic.h"

class Robot {
    public:
        Drive drive;
        Kicker kicker;
        IMU imu;
        PD pd;
        BallLogic ballLogic;
        LineLogic lineLogic;
    
        Robot() : pd(4, 0.1, 200) {
            if(lineNeoOn){
                pinMode(lineNeoPin, OUTPUT);
                digitalWrite(lineNeoPin, HIGH);
            }
        }

        int getMovementAngle(int irAngle, bool irClose, int lineAngle){
            if(lineLogic.getAvoidLineAngle(lineAngle) <= 360) return lineLogic.getAvoidLineAngle(lineAngle);
            if(irClose) return ballLogic.adjustBallAngleClose(irAngle);
            if(!irClose) return irAngle;
            return 0;
        }

        int getPWM(int irAngle, int irDistance, int lineAngle){
            if(lineLogic.lineDetected(lineAngle)) return 100; //line detected
            if(ballLogic.adjustBallAngleClose(irAngle) == irAngle) return 160; // ball in front
            if(ballLogic.distanceClose(irDistance)) return 80 + irDistance * 0.2; // ball close but not in front
            if(ballLogic.ballDetected(irAngle)) return 150; // ball detected
            return 0;
        }

        float getLogicLipoVoltage(){
            return analogRead(logicLipoVoltagePin) /* * (5.0 / 1023.0) * 2 */;
        }

        float getPowerLipoVoltage(){
            return analogRead(powerLipoVoltagePin) /* * (5.0 / 1023.0) * 2 */;
        }
        
    private:
        bool lineNeoOn = true;
        const int lineNeoPin = 23;

        const int logicLipoVoltagePin = 41;
        const int powerLipoVoltagePin = 26;
};

#endif