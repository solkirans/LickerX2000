#ifndef VL53L0XSENSOR_H
#define VL53L0XSENSOR_H

#include <Wire.h>
#include <VL53L0X.h>

// Error Codes
#define VL53L0X_ERR_I2C_INIT_FAIL -1
#define VL53L0X_ERR_SETUP_FAIL -2
#define VL53L0X_ERR_CALIB_FAIL -3
#define VL53L0X_ERR_MEASUREMENT_FAIL -4
#define VL53L0X_ERR_NO_VALID_DATA -5
#define VL53L0X_ERR_I2C_SCAN_MISSING -6

// Configuration
#define VL53L0X_MAX_DISTANCE 1500   // Maximum measurable distance in mm
#define VL53L0X_MIN_DISTANCE 30     // Minimum measurable distance in mm
#define VL53L0X_MEASUREMENT_ARRAY_SIZE 10
#define NUM_SENSORS 1

// I2C Pins
#define I2C_SDA 21
#define I2C_SCL 22

// Global sensor variables
extern VL53L0X VL53L0X_sensors[NUM_SENSORS];
extern uint8_t VL53L0X_shutdownPins[NUM_SENSORS];
extern uint8_t VL53L0X_addresses[NUM_SENSORS];
extern uint16_t VL53L0X_measurementArrays[NUM_SENSORS][VL53L0X_MEASUREMENT_ARRAY_SIZE];
extern uint8_t VL53L0X_measurementIndices[NUM_SENSORS];

// Function Declarations
int VL53L0X_SensorInit();
int VL53L0X_Calibrate();
int VL53L0X_ReadMeasurement();
float VL53L0X_GetAverage(uint8_t sensorIndex);
int VL53L0X_SetupSensors();

#endif
