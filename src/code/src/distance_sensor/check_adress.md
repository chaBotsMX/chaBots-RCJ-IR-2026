/* #include <Arduino.h>
#include <Wire.h>

void setup() {
    Serial.begin(115200);
    while(!Serial && millis() < 4000);
    
    Wire.begin(); // Uses working default configurations
    Serial.println("\n--- I2C Device Scanner ---");
}

void loop() {
    byte error, address;
    int nDevices = 0;

    Serial.println("Scanning bus...");

    for(address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.printf("Device found at address 0x%02X\n", address);
            nDevices++;
        }
        else if (error == 4) {
            Serial.printf("Unknown error at address 0x%02X\n", address);
        }    
    }
    
    if (nDevices == 0) {
        Serial.println("No I2C devices found\n");
    } else {
        Serial.println("Scan complete.\n");
    }

    delay(3000); // Rescan every 3 seconds
} */