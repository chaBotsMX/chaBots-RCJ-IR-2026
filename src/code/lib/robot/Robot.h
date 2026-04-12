#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include "drive/Drive.h"
#include "kicker/Kicker.h"
#include "imu/IMU.h"
#include "pd-control/PD.h"
#include "LineAvoiding.h"
#include "attacker/AttackerControl.h"
#include "goalkeeper/GoalkeeperControl.h"

class Robot {
    public:
        Drive drive;
        Kicker kicker;
        IMU imu;
        PD pd;
        LineAvoiding lineAvoiding;

        AttackerControl attacker;
        GoalkeeperControl goalkeeper;

        MovementCommand atkCmd;
        MovementCommand gkCmd;
    
        Robot() : pd(4, 0.1, 200) {
            if(lineNeoOn){
                pinMode(lineNeoPin, OUTPUT);
                digitalWrite(lineNeoPin, HIGH);
            }
        }

        float getLogicLipoVoltage(){
            return analogRead(logicLipoVoltagePin) /* * (5.0 / 1023.0) * 2 */;
        }

        float getPowerLipoVoltage(){
            return analogRead(powerLipoVoltagePin) /* * (5.0 / 1023.0) * 2 */;
        }
        
        void updateAttackerControl(int irAngle, int irDistance, int lineAngle){
            atkCmd = attacker.calculateMovement(lineAngle, irAngle, irDistance);
        }

        void updateGoalkeeperControl(int irAngle, int irDistance, int lineAngle){
            gkCmd = goalkeeper.calculateMovement(lineAngle, irAngle, irDistance);
        }
        
    private:
        bool lineNeoOn = true;
        const int lineNeoPin = 23;

        const int logicLipoVoltagePin = 41;
        const int powerLipoVoltagePin = 26;
};

#endif