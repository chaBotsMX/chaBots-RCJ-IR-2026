#include <Arduino.h>
#include "ir-sensor/IRSensor.h"
#include "UART.h"

IRSensor ir;
UART uart(Serial7, IRBoard{});

#define IR_UPDATE_TIME 1666

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
  ir.update(833); //update sensors every loop
  
  ir.printIR(100); //print readings every 100ms

  if(millis() > timer){
    timer = millis() + 2;
    if(ir.isBallDetected()){
      angle = ir.getAngle();
      distance = ir.getDistance();

      uart.sendIR(angle, distance);
    }
  }

/*   static unsigned long lastLoop = 0;
  static int actualUpdates = 0;
  static int totalLoops = 0;
  
  unsigned long before = micros();
  ir.update(833);
  unsigned long after = micros();
  
  totalLoops++;
  
  // Check if update actually ran (took time)
  if(after - before > 10){  // If it took >10µs, it ran
    actualUpdates++;
  }
  
  if(millis() - lastLoop > 1000){
    Serial.print("Total loops: ");
    Serial.print(totalLoops);
    Serial.print(" | Actual IR updates: ");
    Serial.println(actualUpdates);
    
    actualUpdates = 0;
    totalLoops = 0;
    lastLoop = millis();
  } */
}