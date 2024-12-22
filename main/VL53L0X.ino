#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include "Arduino.h"

// Constants
const int I2C_SCL_PIN = 22; // I2C clock pin
const int I2C_SDA_PIN = 21; // I2C data pin
const float DistanceSensorMaxValue = 2000.0; // Maximum sensor range in mm
const float DistanceSensorMinValue = 0.0;   // Minimum sensor range in mm
const int MeasDistanceValuesArrayLength = 10;

// Measured values array
float MeasDistanceValues[MeasDistanceValuesArrayLength] = {0.0};
int MeasDistanceCurrentIndex = 0;

// Initialize the VL53L0X sensor
Adafruit_VL53L0X lox;
bool DistanceSensorInitialized = false;
//
extern float calculateAverage(const float* array, size_t length); // Assuming this function exists

// Function to initialize the I2C and distance sensor
int initDistanceSensor() {
    Serial.println("[DEBUG] Initializing I2C communication...");
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    Serial.println("[DEBUG] Initializing VL53L0X sensor...");
    if (!lox.begin()) {
        Serial.println("[ERROR] Sensor not found. Check connections and power.");
        return -1;
    }
    DistanceSensorInitialized = true;
    Serial.println("[DEBUG] Performing sensor health check...");
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);

    if (measure.RangeStatus != 0) {
        Serial.println("[ERROR] Sensor failed initial health check.");
        return -2;
    }

    Serial.println("[INFO] Sensor initialized successfully.");
    return 0;
}

// Function to read data from the distance sensor
float readDistanceSensor() {
    Serial.println("[DEBUG] Reading distance sensor...");

    VL53L0X_RangingMeasurementData_t measure;
    if (!DistanceSensorInitialized) {
        Serial.println("[ERROR] Sensor not initialized or disabled.");
        return -1.0;
    }

    lox.rangingTest(&measure, false);

    if (measure.RangeStatus != 0) {
        Serial.println("[ERROR] Sensor returned invalid data.");
        return -2.0;
    }

    float distance = static_cast<float>(measure.RangeMilliMeter);

    if (distance < DistanceSensorMinValue) {
        Serial.printf("[WARNING] Measured value %.2f below minimum threshold.\n", distance);
        return -3.0;
    }

    if (distance > DistanceSensorMaxValue) {
        Serial.printf("[WARNING] Measured value %.2f above maximum threshold.\n", distance);
        return -4.0;
    }

    Serial.printf("[INFO] Measured distance: %.2f mm.\n", distance);
    return distance;
}

// Function to get the filtered distance value
float getFilteredDistance() {
    Serial.println("[DEBUG] Fetching filtered distance...");

    float readValue = readDistanceSensor();
    if (readValue < 0) {
        Serial.printf("[ERROR] Read function returned error code: %.2f\n", readValue);
        return readValue;
    }

    MeasDistanceValues[MeasDistanceCurrentIndex] = readValue;
    MeasDistanceCurrentIndex = (MeasDistanceCurrentIndex + 1) % MeasDistanceValuesArrayLength;

    Serial.println("[DEBUG] Calculating average...");
    float average = calculateAverage(MeasDistanceValues, MeasDistanceValuesArrayLength);
    Serial.printf("[INFO] Filtered distance: %.2f mm.\n", average);

    return average;
}

// Error code meanings:
// -1: Sensor not initialized or not found
// -2: Sensor returned invalid data during read operation
// -3: Measured value below minimum threshold
// -4: Measured value above maximum threshold
