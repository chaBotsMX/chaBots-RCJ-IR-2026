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

        MovementCommandAtk atkCmd;
        MovementCommandGk gkCmd;
    
        Robot() : pd(4, 0.1, 200) {
            pinMode(lineNeoPin, OUTPUT);
            pinMode(button1Pin, INPUT); pinMode(button2Pin, INPUT);
        }

        float getLogicLipoVoltage(){
            return analogRead(logicLipoVoltagePin); //((analogRead(logicLipoVoltagePin) * 3.3) / 1023.0) * 2 ;
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

        bool hasBall(int irAngle, int irDistance){
            static unsigned long ballSeenSince = 0;
            static bool tracking = false;

            bool currentBallState = ((irAngle >= 85 and irAngle <= 95) and irDistance < 100); // Ball in front and close

            if (currentBallState) {
                if (!tracking) {
                    ballSeenSince = millis();
                    tracking = true;
                }
                if (millis() - ballSeenSince >= 80) {
                    return true;
                }
            } else {
                tracking = false;
                ballSeenSince = 0;
            }

            return false;
        }

        bool wasButton1Pressed(){
            static bool lastState = false;
            bool currentState = digitalRead(button1Pin);
            bool pressed = (currentState && !lastState);
            lastState = currentState;
            return pressed;
        }

        bool wasButton2Pressed(){
            static bool lastState = false;
            bool currentState = digitalRead(button2Pin);
            bool pressed = (currentState && !lastState);
            lastState = currentState;
            return pressed;
        }
        
        void buzz(int freq, int time){
            tone(buzzerPin, freq, time);
        }

        void setLineNeo(bool on){
            digitalWrite(lineNeoPin, on ? HIGH : LOW);
        }

        int getYawCorrection(){
            float setpoint = 0; // Desired yaw angle (e.g., facing forward)
            float currentYaw = imu.getYaw();
            if(wasButton2Pressed()) setpoint = currentYaw;
            return pd.getCorrection(currentYaw - setpoint);
        }
        
    private:
        const int lineNeoPin = 23;

        const int logicLipoVoltagePin = 41;
        const int powerLipoVoltagePin = 26;

        const int button1Pin = 30;
        const int button2Pin = 27;

        const int buzzerPin = 4;
};

#endif