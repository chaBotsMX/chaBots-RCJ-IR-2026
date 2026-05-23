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
  uart.beginLine(2000000);
}

void loop() {
  ls.update();
  ls.printLS(100); //print readings every 100ms

  angle = ls.getAngle();
  uart.sendLine(angle);
}