#include <Arduino.h>
#include "ir-sensor/IRSensor.h"
#include "UART.h"

IRSensor ir;
UART uart(Serial7, IRBoard{});

#define IR_UPDATE_TIME 833

int angle = 0;
int distance = 0;

unsigned long timer = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("hi");
  uart.begin(1000000);
  delay(1000);
}

void loop() {
  ir.update(IR_UPDATE_TIME); //update sensors every loop
  
  ir.printIR(100); //print readings every 100ms

  if(millis() > timer){
    timer = millis() + 2;
    if(ir.isBallDetected()){
      angle = ir.getAngle();
      distance = ir.getDistance();

      uart.sendIR(angle, distance);
    }
  }
}