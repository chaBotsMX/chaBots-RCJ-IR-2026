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
            pinMode(button1Pin, INPUT_PULLDOWN); pinMode(button2Pin, INPUT_PULLDOWN);
        }

        float getLogicLipoVoltage(){
            return analogRead(logicLipoVoltagePin); //((analogRead(logicLipoVoltagePin) * 3.3) / 1023.0) * 2 ;
        }

        float getPowerLipoVoltage(){
            return analogRead(powerLipoVoltagePin) /* * (5.0 / 1023.0) * 2 */;
        }
        
        void updateAttackerControl(int irAngle, int irDistance, int lineAngle, int cameraAngle){
            atkCmd = attacker.calculateMovement(lineAngle, irAngle, irDistance, cameraAngle);
        }

        void updateGoalkeeperControl(int irAngle, int irDistance, int lineAngle, int cameraAngle){
            gkCmd = goalkeeper.calculateMovement(lineAngle, irAngle, irDistance, cameraAngle);
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

        void updateButtons(){
            button1State = digitalRead(button1Pin);
            button2State = digitalRead(button2Pin);

            if (button1State && !lastButton1State) {
                button1Toggle = !button1Toggle;
                buzz(1000, 100);
            }

            if (button2State && !lastButton2State) {
                button2Toggle = !button2Toggle;
                buzz(1000, 100);
            }

            lastButton1State = button1State;
            lastButton2State = button2State;
        };
        
        void buzz(int freq, int time){
            tone(buzzerPin, freq, time);
        }

        void setLineNeo(bool on){
            digitalWrite(lineNeoPin, on ? HIGH : LOW);
        }

        bool wasButton1Pressed(){
            return button1Toggle;
        }

        bool wasButton2Pressed(){
            return button2Toggle;
        }

        int getYawCorrection(int cameraAngle, int irAngle){
            float setpoint = 0; // Desired yaw angle (e.g., facing forward)
            float currentYaw = imu.getYaw();
            if(wasButton1Pressed()) setpoint = currentYaw;
            float error = currentYaw - setpoint;
            float offset = 0;
            if(attacker.isBallOnFront(irAngle) and cameraAngle <= 140){
                if(cameraAngle < 60) offset = 40; // Ball on left, turn slightly left
                else if(cameraAngle > 80) offset = -40; // Ball on right, turn slightly right
                else offset = 0;
            };
            return pd.getCorrection(error - offset);
        }
        
    private:
        const int lineNeoPin = 23;

        const int logicLipoVoltagePin = 41;
        const int powerLipoVoltagePin = 26;

        const int button1Pin = 30;
        const int button2Pin = 27;

        const int buzzerPin = 4;

        bool lastButton1State = false;
        bool lastButton2State = false;
        bool button1State = false;
        bool button2State = false;
        bool button1Toggle = false;
        bool button2Toggle = false;
};

#endif