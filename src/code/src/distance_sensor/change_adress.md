/* #include <Arduino.h>
#include <Wire.h>

// ==========================================
// CONFIGURATION: CHANGE THIS FOR EACH SENSOR
// ==========================================
const uint8_t DEFAULT_ADDRESS = 0x11; // Factory default
const uint8_t NEW_ADDRESS     = 0x12; // Target address (Change to 0x13, 0x14, etc.)

void setup() {
    Serial.begin(115200);
    while(!Serial && millis() < 4000); // Wait for Serial Monitor to open
    
    Wire.begin();
    Wire.setClock(100000); // Standard speed is safest for address flashing
    
    Serial.println("==========================================");
    Serial.printf("Attempting to change URM09 address from 0x%02X to 0x%02X\n", DEFAULT_ADDRESS, NEW_ADDRESS);
    Serial.println("Ensure ONLY ONE sensor is connected to the I2C bus.");
    Serial.println("==========================================");
    delay(2000);

    // Step 1: Verify the sensor is actually alive at the default address
    Wire.beginTransmission(DEFAULT_ADDRESS);
    if (Wire.endTransmission() != 0) {
        Serial.printf("[ERROR] No sensor detected at default address 0x%02X!\n", DEFAULT_ADDRESS);
        Serial.println("Check your wiring, or perhaps this sensor's address was already changed.");
        return;
    }
    Serial.println("[SUCCESS] Found sensor at default address.");

    // Step 2: Write the new address to Register 0x00
    Wire.beginTransmission(DEFAULT_ADDRESS);
    Wire.write(0x00);        // eSLAVEADDR_INDEX register
    Wire.write(NEW_ADDRESS); // The new I2C address destination
    if (Wire.endTransmission() == 0) {
        Serial.println("[SUCCESS] New address written to sensor memory.");
    } else {
        Serial.println("[ERROR] Failed to transmit new address command.");
        return;
    }

    // Step 3: Wait for the sensor's internal EEPROM to flash and reboot
    Serial.println("Waiting for sensor internal reset...");
    delay(500); 

    // Step 4: Scan the bus to verify it responds to the new address
    Wire.beginTransmission(NEW_ADDRESS);
    if (Wire.endTransmission() == 0) {
        Serial.println("\n******************************************");
        Serial.printf(" CONFIRMED! Sensor successfully moved to: 0x%02X\n", NEW_ADDRESS);
        Serial.println("******************************************");
        Serial.println("You can now unplug this sensor, mark it, and connect the next one.");
    } else {
        Serial.println("\n[ERROR] Sensor did not respond at the new address. Flash failed or address out of range.");
    }
}

void loop() {
    // Keep loop empty
} */