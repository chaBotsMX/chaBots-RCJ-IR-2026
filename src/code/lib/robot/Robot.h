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
#include "display/Display.h"

class Robot {
    public:
        Drive drive;
        Kicker kicker;
        IMU imu;
        PD pd;
        LineAvoiding lineAvoiding;
        
        Display display;
    
        Robot() : pd(4, 0.1, 180) {
            pinMode(lineNeoPin, OUTPUT);
            pinMode(button1Pin, INPUT); pinMode(button2Pin, INPUT);
        }

        float getLogicLipoVoltage(){
            return map(float(analogRead(logicLipoVoltagePin)), 0.0, 1023.0, 0.0, 3.3) * 2;
        }

        float getPowerLipoVoltage(){
            return map(float(analogRead(powerLipoVoltagePin)), 0.0, 1023.0, 0.0, 3.3) * 5;
        }

        bool hasBall(int irAngle, int irDistance){
            static unsigned long ballSeenSince = 0;
            static bool tracking = false;

            bool currentBallState = ((irAngle >= 80 and irAngle <= 100) and irDistance < 20); // Ball in front and close

            if (currentBallState) {
                if (!tracking) {
                    ballSeenSince = millis();
                    tracking = true;
                }
                if (millis() - ballSeenSince >= 120) {
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

        int getYawCorrection(float setpoint = 0) { // Desired yaw angle (e.g., facing forward)
            float currentYaw = imu.getYaw();
            //if(wasButton1Pressed()) setpoint = currentYaw;
            float error = currentYaw - setpoint;
            return pd.getCorrection(error);
        }

        void displayVoltage(){
            display.clear();
            display.showText(String(getLogicLipoVoltage()) + "V", 5, 5, 2);
            display.showText("Logic", 5, 25, 1);
            display.showText(String(getPowerLipoVoltage()) + "V", 5, 35, 2);
            display.showText("Power", 5, 55, 1);
        }

        void updateDisplay() {
            bool currentButtonState = wasButton2Pressed();
            unsigned long currentTime = millis();
            
            // Update display every 500ms OR when button state changes
            if (currentButtonState != lastButtonState || (currentTime - displayUpdateTimer >= 5000)) {
                displayUpdateTimer = currentTime;
                lastButtonState = currentButtonState;
                isPlaying = currentButtonState;
                
                if (isPlaying) {
                    display.showImage();
                } else {
                    displayVoltage();
                }
            }
        }
        
    private:
        const int lineNeoPin = 23;

        const int logicLipoVoltagePin = 41;
        const int powerLipoVoltagePin = 26;

        const int button1Pin = 32;
        const int button2Pin = 33;

        const int buzzerPin = 4;

        bool lastButton1State = false;
        bool lastButton2State = false;
        bool button1State = false;
        bool button2State = false;
        bool button1Toggle = false;
        bool button2Toggle = false;

        unsigned long displayUpdateTimer = 0;
        bool lastButtonState = false;
        bool isPlaying = false; // Track current display mode
};

#endif