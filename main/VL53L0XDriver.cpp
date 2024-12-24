#include "VL53L0XOccupancy.h"


// Variables
VL53L0X VL53L0X_sensor;
uint16_t VL53L0X_measurementArray[VL53L0X_MEASUREMENT_ARRAY_SIZE] = {0};
uint8_t VL53L0X_measurementIndex = 0;
bool VL53L0X_initialized = false;


int VL53L0X_Init() {
  Serial.println("[INFO] Initializing VL53L0X sensor...");
  
  if (!VL53L0X_sensor.init()) {
    Serial.println("[ERROR] Failed to initialize VL53L0X sensor!");
    return VL53L0X_ERR_INIT_FAIL;
  }

  if (VL53L0X_Calibrate() != 0) {
    Serial.println("[ERROR] Sensor calibration failed!");
    return VL53L0X_ERR_CALIB_FAIL;
  }

  VL53L0X_initialized = true;
  Serial.println("[INFO] VL53L0X sensor initialized successfully.");
  return 0;
}

int VL53L0X_Calibrate() {
  Serial.println("[INFO] Calibrating VL53L0X sensor for long-range mode...");
  
  VL53L0X_sensor.setSignalRateLimit(0.1); // Reduce false positives
  VL53L0X_sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  VL53L0X_sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  VL53L0X_sensor.setMeasurementTimingBudget(200000); // Long-range timing

  if (!VL53L0X_sensor.setMeasurementTimingBudget(200000)) {
    Serial.println("[ERROR] Calibration settings failed!");
    return VL53L0X_ERR_CALIB_FAIL;
  }

  Serial.println("[INFO] VL53L0X sensor calibrated successfully.");
  return 0;
}

int VL53L0X_ReadMeasurement() {
  Serial.println("[INFO] Reading measurement from VL53L0X sensor...");

  if (!VL53L0X_initialized) {
    Serial.println("[ERROR] Sensor not initialized!");
    return VL53L0X_ERR_INIT_FAIL;
  }

  uint16_t currentDistance = VL53L0X_sensor.readRangeSingleMillimeters();

  if (VL53L0X_sensor.timeoutOccurred()) {
    Serial.println("[ERROR] Timeout occurred during measurement!");
    return VL53L0X_ERR_MEASUREMENT_FAIL;
  }

  if (currentDistance < VL53L0X_MIN_DISTANCE || currentDistance > VL53L0X_MAX_DISTANCE) {
    Serial.println("[WARNING] Measured distance out of range!");
    return VL53L0X_ERR_INVALID_DISTANCE;
  }

  VL53L0X_measurementArray[VL53L0X_measurementIndex] = currentDistance;
  VL53L0X_measurementIndex = (VL53L0X_measurementIndex + 1) % VL53L0X_MEASUREMENT_ARRAY_SIZE;

  Serial.print("[INFO] Measurement successful: ");
  Serial.println(currentDistance);
  return currentDistance;
}

float VL53L0X_GetAverage() {
  Serial.println("[INFO] Calculating average of measurements...");
  
  if (!VL53L0X_initialized) {
    Serial.println("[ERROR] Sensor not initialized!");
    return VL53L0X_ERR_INIT_FAIL;
  }

  uint32_t sum = 0;
  uint8_t validDataCount = 0;

  for (uint8_t i = 0; i < VL53L0X_MEASUREMENT_ARRAY_SIZE; i++) {
    if (VL53L0X_measurementArray[i] > 0) { // Only consider valid data
      sum += VL53L0X_measurementArray[i];
      validDataCount++;
    }
  }

  if (validDataCount == 0) {
    Serial.println("[ERROR] No valid data for average calculation!");
    return VL53L0X_ERR_NO_VALID_DATA;
  }

  float average = (float)sum / validDataCount;
  Serial.print("[INFO] Average distance: ");
  Serial.println(average);
  return average;
}