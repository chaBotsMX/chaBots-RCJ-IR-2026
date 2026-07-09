#include <Arduino.h>
#include "distance-sensor/DistanceSensor.h"
#include "Localization.h"
#include "UART.h"

DistanceSensor distanceSensor;
Localization localization;
UART uart(Serial1, XIAO{});

int posX = 254, posY = 254;
int frontDistance = 999, backDistance = 999, leftDistance = 999, rightDistance = 999;

void setup() {
    Serial.begin(115200);
    Serial.println("Distance Sensor Test Starting...");
    
    distanceSensor.begin();
    uart.beginDistance(2000000);
}

void loop() {
    distanceSensor.update();

    frontDistance = distanceSensor.getFrontDistance();
    backDistance = distanceSensor.getBackDistance();
    leftDistance = distanceSensor.getLeftDistance();
    rightDistance = distanceSensor.getRightDistance();

    localization.update(frontDistance, backDistance, leftDistance, rightDistance);

    posX = localization.getX();
    posY = localization.getY();

    distanceSensor.printDistance(10); // Print every 10ms
    uart.sendDistance(posX, posY);
}