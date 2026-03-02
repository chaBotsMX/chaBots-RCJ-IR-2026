#include <Arduino.h>
#include "IRSensor.h"

IRSensor ir;

#define IR_UPDATE_TIME 0

int angle = 0;
int intensity = 0;
int distance = 0;

unsigned long timer = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("hi");
  delay(1000);
}

void loop() {
  ir.update(IR_UPDATE_TIME);
  
  if(millis() > timer){
    timer = millis() + 200;
    
    ir.printIR();
  }
}