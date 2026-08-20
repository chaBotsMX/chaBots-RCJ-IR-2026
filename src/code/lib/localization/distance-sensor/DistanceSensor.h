/**
 * @file DistanceSensor.h
 * @brief Non-blocking management for 4 URM09 sensors in a rectangular field configuration.
 * @author Alfonso De Anda / chaBotsMX
 * @date 05/19/26
 */

#ifndef DistanceSensor_H
#define DistanceSensor_H

#include <Arduino.h>
#include <Wire.h>

// ============================================================================
// SENSOR ENABLE/DISABLE - Set which individual sensors are connected
// ============================================================================
#define ENABLE_FRONT    false   // Pair 0, Sensor 1
#define ENABLE_BACK     true    // Pair 0, Sensor 2
#define ENABLE_LEFT     false   // Pair 1, Sensor 1
#define ENABLE_RIGHT    true    // Pair 1, Sensor 2
// ============================================================================

class DistanceSensor {
  public:
    DistanceSensor();
    
    void begin();
    
    void update();
    
    // Debug
    void printDistance(unsigned long timeLimit, int posX = 999, int posY = 999);

    int16_t getFrontDistance();
    int16_t getBackDistance();
    int16_t getLeftDistance();
    int16_t getRightDistance();

  private:
    // Structure to handle paired configurations
    struct SensorPair {
        uint8_t addr1;         // First sensor I2C address
        uint8_t addr2;         // Opposite sensor I2C address
        uint8_t configByte;    // Mode and Range configuration (0x07 register)
        unsigned long delayMs; // Required physical wave flight timeout
        bool enabled1;         // Enable first sensor
        bool enabled2;         // Enable second sensor
    };

    // Sensor mapping parameters
    SensorPair pairs[2] = {
        // Pair 0: Front & Back. Field dimension needs ~230cm -> config 300cm (0x10), 25ms flight time
        {0x11, 0x12, 0x10, 25, ENABLE_FRONT, ENABLE_BACK}, 
        // Pair 1: Left & Right. Field dimension needs ~140cm -> config 150cm (0x00), 15ms flight time
        {0x13, 0x14, 0x00, 15, ENABLE_LEFT, ENABLE_RIGHT}
    };

    // State machine management
    enum State { TRIGGER_PAIR, WAIT_FOR_ECHO };
    State currentState;
    int currentPairIndex;
    unsigned long stateTimer;

    // Direct readings storage (0: Front, 1: Back, 2: Left, 3: Right)
    int16_t rawReadings[4];
    
    // 3-Sample History Matrix for the Median Filters
    int16_t filterBuffer[4][3];
    int16_t filteredReadings[4];

    // Core internal routines
    void triggerPairPings(const SensorPair& pair);
    void readPairData(const SensorPair& pair, int index1, int index2);
    int16_t requestRegisterValue(uint8_t address);
    int16_t computeMedian(int16_t samples[]);
};

#endif