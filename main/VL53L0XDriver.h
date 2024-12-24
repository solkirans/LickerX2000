#ifndef VL53L0X_DRIVER_H
#define VL53L0X_DRIVER_H

  #include <Wire.h>
  #include <VL53L0X.h>

  // Error Codes
  #define VL53L0X_ERR_INIT_FAIL -1
  #define VL53L0X_ERR_CALIB_FAIL -2
  #define VL53L0X_ERR_MEASUREMENT_FAIL -3
  #define VL53L0X_ERR_INVALID_DISTANCE -4
  #define VL53L0X_ERR_NO_VALID_DATA -5

  // Parameters
  #define VL53L0X_MAX_DISTANCE 2000   // Maximum measurable distance in mm
  #define VL53L0X_MIN_DISTANCE 5     // Minimum measurable distance in mm
  #define VL53L0X_MEASUREMENT_ARRAY_SIZE 10 // Measurement array size

  // Variables
  extern VL53L0X VL53L0X_sensor;
  extern uint16_t VL53L0X_measurementArray[];
  extern uint8_t VL53L0X_measurementIndex;
  extern bool VL53L0X_initialized;

  // Function Prototypes
  int VL53L0X_Init();
  int VL53L0X_Calibrate();
  int VL53L0X_ReadMeasurement();
  float VL53L0X_GetAverage();
#endif