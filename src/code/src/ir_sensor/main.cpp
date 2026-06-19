#include <Arduino.h>
#include "ir-sensor/IRSensor.h"
#include "UART.h"

IRSensor ir;
UART uart(Serial7, IRBoard{});

#define IR_UPDATE_TIME 1600

int angle = 0;
int distance = 0;

unsigned long timer = 0;

void setup() {
  delay(200);

  Serial.begin(115200);
  Serial.println("hi");
  uart.beginIR(2000000);
}

void loop() {
  ir.update(IR_UPDATE_TIME); //update sensors every loop
  
  ir.printIR(100); //print readings every 100ms

  angle = ir.getAngle();
  distance = ir.getRelativeDistance();

  uart.sendIR(angle/2, distance);
}