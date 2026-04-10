#include <Arduino.h>
#include "ir-sensor/IRSensor.h"
#include "UART.h"

IRSensor ir;
UART uart(Serial7, IRBoard{});

#define IR_UPDATE_TIME 600

int angle = 0;
int isBallClose = 0;

unsigned long timer = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("hi");
  uart.begin(2000000);
  delay(1000);
}

void loop() {
  ir.update(IR_UPDATE_TIME); //update sensors every loop
  
  ir.printIR(100); //print readings every 100ms

  if(millis() > timer){
    timer = millis() + 4;
    angle = ir.getAngle();
    isBallClose = ir.isBallClose();

    uart.sendIR(angle/2, isBallClose);
  }
}