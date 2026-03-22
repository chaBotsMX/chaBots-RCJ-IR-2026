#include <Arduino.h>
#include "line-sensor/LineSensor.h"
#include "UART.h"

LineSensor ls;
UART uart(Serial3, LineBoard{});

int angle = 0;

unsigned long timer;

void setup() {
  Serial.begin(115200);
  ls.begin();
  uart.begin(1000000);
  delay(1000);
}

void loop() {
    ls.update();
    ls.printLS(100); //print readings every 100ms

    if(millis() > timer){
        timer = millis() + 2;

        if(ls.isLineDetected()){
            angle = ls.getAngle();
            uart.sendLine(angle);
        }
    }
}