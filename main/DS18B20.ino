#include <OneWire.h>
#include <DallasTemperature.h>
#include "Arduino.h"

// Constants
const int ONE_WIRE_PIN = 4; // Pin for OneWire communication
const float SensorMaxValue = 125.0; // Maximum sensor range in Celsius
const float SensorMinValue = -55.0; // Minimum sensor range in Celsius
const int MeasuredValuesArrayLength = 10;

// Measured values array
float measuredValues[MeasuredValuesArrayLength] = {0.0};
int currentIndex = 0;

// Initialize OneWire and DallasTemperature libraries
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

// Function to initialize the temperature sensor
int initTemperatureSensor() {
    Serial.println("[DEBUG] Initializing OneWire communication...");
    sensors.begin();

    if (sensors.getDeviceCount() == 0) {
        Serial.println("[ERROR] No devices found on the OneWire bus.");
        return -1;
    }

    Serial.println("[INFO] Temperature sensor initialized successfully.");
    return 0;
}

// Function to read data from the temperature sensor
float readTemperatureSensor() {
    Serial.println("[DEBUG] Reading temperature sensor...");

    sensors.requestTemperatures(); // Request temperatures from the sensor
    float tempC = sensors.getTempCByIndex(0);

    if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.println("[ERROR] Sensor disconnected.");
        return -1.0;
    }

    if (tempC < SensorMinValue) {
        Serial.printf("[WARNING] Measured value %.2f below minimum threshold.\n", tempC);
        return -2.0;
    }

    if (tempC > SensorMaxValue) {
        Serial.printf("[WARNING] Measured value %.2f above maximum threshold.\n", tempC);
        return -3.0;
    }

    Serial.printf("[INFO] Measured temperature: %.2f Celsius.\n", tempC);
    return tempC;
}

// Function to get the filtered temperature value
float getFilteredTemperature() {
    Serial.println("[DEBUG] Fetching filtered temperature...");

    float readValue = readTemperatureSensor();
    if (readValue < 0) {
        Serial.printf("[ERROR] Read function returned error code: %.2f\n", readValue);
        return readValue;
    }

    measuredValues[currentIndex] = readValue;
    currentIndex = (currentIndex + 1) % MeasuredValuesArrayLength;

    Serial.println("[DEBUG] Calculating average...");
    extern float calculateAverage(const float* array, size_t length); // Assuming this function exists
    float average = calculateAverage(measuredValues, MeasuredValuesArrayLength);
    Serial.printf("[INFO] Filtered temperature: %.2f Celsius.\n", average);

    return average;
}

// Error code meanings:
// -1: No devices found on the OneWire bus
// -2: Measured value below minimum threshold
// -3: Measured value above maximum threshold
