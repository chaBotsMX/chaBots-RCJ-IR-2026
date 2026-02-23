#include <Arduino.h>

#include "IRSensor.h"

IRSensor ir;

#define IR_UPDATE_TIME 833

int angle = 0;
int intensity = 0;
int distance = 0;

unsigned long timer;

unsigned long lastTime;

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  //unsigned long loopTime = micros() - lastTime;
  //lastTime = micros();

  ir.update(IR_UPDATE_TIME);
  
  ir.printIR(ir.getAngle(), 0, 1000, true);

  //Serial.println("1");
}