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

        int getMovementAngle(int irAngle, int irClose, int lineAngle){
            if(lineLogic.lineDetected(lineAngle)) return lineLogic.getAvoidLineAngle(lineAngle);
            if(irClose == 1) return ballLogic.adjustBallAngleClose(irAngle);
            if(irClose == 0) return irAngle;
            return 0;
        }

        int getPWM(int irAngle, int irClose, int lineAngle){
            if(ballLogic.adjustBallAngleClose(irAngle) == irAngle) return 210;
            if(irClose == 1) return 150;
            if(irClose == 0) return 200;
            if(lineLogic.lineDetected(lineAngle)) return 150;
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