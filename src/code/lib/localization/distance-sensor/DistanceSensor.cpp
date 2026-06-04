/**
 * @file DistanceSensor.cpp
 * @brief URM09 Distance Sensor I2C Interface Implementation
 * @author Alfonso De Anda / chaBotsMX
 * @date 05/19/26
 */

#include "DistanceSensor.h"

DistanceSensor::DistanceSensor() {
    currentState = TRIGGER_PAIR;
    currentPairIndex = 0;
    stateTimer = 0;

    // Initialize all reading states cleanly to zero
    for (int i = 0; i < 4; i++) {
        rawReadings[i] = 0;
        filteredReadings[i] = 0;
        for (int j = 0; j < 3; j++) {
            filterBuffer[i][j] = 0;
        }
    }
}

void DistanceSensor::begin() {
    Wire.begin();
    Wire.setClock(400000); // 400kHz Fast I2C Execution 

    // Configure pairs according to physical field depth constraints
    for (int i = 0; i < 2; i++) {
        // Write configurations to Address 1
        Wire.beginTransmission(pairs[i].addr1);
        Wire.write(0x07); // eCFG_INDEX register
        Wire.write(pairs[i].configByte);
        Wire.endTransmission();
        delay(1); // Physical write hardware buffer delay

        // Write configurations to Address 2
        Wire.beginTransmission(pairs[i].addr2);
        Wire.write(0x07); 
        Wire.write(pairs[i].configByte);
        Wire.endTransmission();
        delay(1);
    }
    
    stateTimer = millis();
}

void DistanceSensor::update() {
    const SensorPair& activePair = pairs[currentPairIndex];

    switch (currentState) {
        case TRIGGER_PAIR:
            triggerPairPings(activePair);
            stateTimer = millis();
            currentState = WAIT_FOR_ECHO;
            break;

        case WAIT_FOR_ECHO:
            // Non-blocking dynamic flight delay tracking matching current pair constraints
            if (millis() - stateTimer >= activePair.delayMs) {
                
                // Map local pair readings to absolute array indices
                if (currentPairIndex == 0) {
                    readPairData(activePair, 0, 1); // Indices 0 (Front), 1 (Back)
                } else {
                    readPairData(activePair, 2, 3); // Indices 2 (Left), 3 (Right)
                }

                // Advance system loop index to process opposite cross-axis 
                currentPairIndex = (currentPairIndex + 1) % 2;
                currentState = TRIGGER_PAIR;
            }
            break;
    }
}

void DistanceSensor::triggerPairPings(const SensorPair& pair) {
    // Simultaneously request triggers across both independent paths 
    Wire.beginTransmission(pair.addr1);
    Wire.write(0x08); // eCMD_INDEX
    Wire.write(0x01); // CMD_DISTANCE_MEASURE
    Wire.endTransmission();

    Wire.beginTransmission(pair.addr2);
    Wire.write(0x08); 
    Wire.write(0x01); 
    Wire.endTransmission();
}

void DistanceSensor::readPairData(const SensorPair& pair, int index1, int index2) {
    // Gather newest hardware measurements
    rawReadings[index1] = requestRegisterValue(pair.addr1);
    rawReadings[index2] = requestRegisterValue(pair.addr2);

    // Shift window variables down to insert new data inside 3-sample median array
    for (int idx : {index1, index2}) {
        filterBuffer[idx][2] = filterBuffer[idx][1];
        filterBuffer[idx][1] = filterBuffer[idx][0];
        filterBuffer[idx][0] = rawReadings[idx];

        // Process samples to filter out instantaneous spike errors/glitches
        filteredReadings[idx] = computeMedian(filterBuffer[idx]);
    }
}

int16_t DistanceSensor::requestRegisterValue(uint8_t address) {
    Wire.beginTransmission(address);
    Wire.write(0x03); // eDIST_H_INDEX
    if (Wire.endTransmission() != 0) {
        return 999; // Fallback loop flag handling I2C bus breaks or disconnections
    }

    uint8_t receivedBytes = Wire.requestFrom(address, (uint8_t)2);
    if (receivedBytes == 2) {
        uint8_t highByte = Wire.read();
        uint8_t lowByte = Wire.read();
        int16_t calculatedVal = (int16_t)((highByte << 8) | lowByte);
        
        // Handle out of bounds/failure error readings coming directly from URM09 hardware
        if (calculatedVal == 0 || calculatedVal > 65000) return 999; 
        return calculatedVal;
    }
    return 999;
}

int16_t DistanceSensor::computeMedian(int16_t samples[]) {
    int16_t a = samples[0];
    int16_t b = samples[1];
    int16_t c = samples[2];
    
    // Fast conditional swapping network to locate the midpoints without looping overhead
    if ((a <= b && b <= c) || (c <= b && b <= a)) return b;
    if ((b <= a && a <= c) || (c <= a && a <= b)) return a;
    return c;
}

// User Abstraction Fetch Layouts
int16_t DistanceSensor::getFrontDistance() { return filteredReadings[0]; }
int16_t DistanceSensor::getBackDistance()  { return filteredReadings[1]; }
int16_t DistanceSensor::getLeftDistance()  { return filteredReadings[2]; }
int16_t DistanceSensor::getRightDistance() { return filteredReadings[3]; }

void DistanceSensor::printDistance(unsigned long timeLimit) {
    static unsigned long lastPrintTime = 0;
    if (millis() - lastPrintTime >= timeLimit) {
        lastPrintTime = millis();
        Serial.print("FRONT: ");   Serial.print(filteredReadings[0]); Serial.print("cm | ");
        Serial.print("BACK: ");    Serial.print(filteredReadings[1]); Serial.print("cm | ");
        Serial.print("LEFT: ");    Serial.print(filteredReadings[2]); Serial.print("cm | ");
        Serial.print("RIGHT: ");   Serial.print(filteredReadings[3]); Serial.println("cm");
    }
}