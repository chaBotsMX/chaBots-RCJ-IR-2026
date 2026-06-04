#include <Arduino.h>
#include "DistanceSensor.h"

DistanceSensor distanceSensor;

void setup() {
    Serial.begin(115200);
    Serial.println("Distance Sensor Test Starting...");
    
    distanceSensor.begin();
}

void loop() {
    distanceSensor.update();
    distanceSensor.printDistance(100); // Print every 100ms
}