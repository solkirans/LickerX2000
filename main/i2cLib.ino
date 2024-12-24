#include <Wire.h>


// Constants and Paremeters
const int I2C_SCL_PIN = 22; // I2C clock pin
const int I2C_SDA_PIN = 21; // I2C data pin
const unsigned int I2C_retryLimit = 5; // Maximum number of retries
const unsigned int I2C_retryIntervalMs = 250; // Wait 100ms between retries

// Error Codes
#define I2C_ERROR_ALREADY_INITIALIZED -1
#define I2C_ERROR_PIN_CONFIGURATION_FAILED -2
#define I2C_ERROR_BUS_INITIALIZATION_FAILED -3
#define I2C_ERROR_UNKNOWN -99

//Variables
bool I2C_InitStatus = false; // Tracks I2C initialization status



// Function to initialize I2C with retry mechanism
int I2C_init() {
    if (I2C_InitStatus) {
        // [INFO] I2C is already initialized, no action needed
        Serial.println("[INFO] I2C is already initialized.");
        return I2C_ERROR_ALREADY_INITIALIZED;
    }

    // Configure I2C pins
    if (I2C_SCL_PIN < 0 || I2C_SDA_PIN < 0) {
        // [ERROR] Invalid pin configuration
        Serial.println("[ERROR] Invalid I2C pin configuration.");
        return I2C_ERROR_PIN_CONFIGURATION_FAILED;
    }

    int I2C_attempt = 0; // Tracks the number of retry attempts

    while (I2C_attempt < I2C_retryLimit) {
        Serial.print("[INFO] Attempt ");
        Serial.print(I2C_attempt + 1);
        Serial.println(" to initialize I2C bus.");

        // Initialize I2C
        Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

        // Perform a quick I2C health check (e.g., scan for devices)
        Wire.beginTransmission(0x29); // Send to an invalid address to validate the bus
        if (Wire.endTransmission() == 0) {
            // [INFO] I2C bus initialized successfully
            I2C_InitStatus = true;
            Serial.println("[INFO] I2C bus initialized successfully.");
            return 0; // Success
        } else {
            // [ERROR] I2C bus is not operational
            Serial.println("[ERROR] I2C bus initialization failed. Retrying...");
        }

        // Wait before the next retry
        delay(I2C_retryIntervalMs);
        I2C_attempt++;
    }

    // If retries exhausted, return error
    Serial.println("[ERROR] I2C initialization failed after maximum retries.");
    return I2C_ERROR_BUS_INITIALIZATION_FAILED;
}
