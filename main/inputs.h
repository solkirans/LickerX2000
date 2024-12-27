#ifndef INPUTS_H
#define INPUTS_H

#include <Bounce2.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Global Variable Definitions
extern int8_t TempSensorStatus;            // Temperature sensor status
extern int8_t ButtonStatus;                // Button status output
extern int8_t ButtonPressType;             // Button press type
extern unsigned long ButtonPressTime;      // Button press time in ms
extern bool ButtonPressTypeDataReady;      // Button press type data ready flag
extern bool ButtonPressTypeDataClearReq;   // Button press type data clear request input

// Constant Definitions
#define TempSensorPin 32
#define ButtonPin 33
#define ButtonDebounceFilterTime 100       // ms
#define ButtonStatusTimeout 200            // ms
#define ButtonStatus_NA 0
#define ButtonStatus_Pressed 1
#define ButtonStatus_Released 2
#define ButtonPressType_NA 0
#define ButtonPressType_Short 1
#define ButtonPressType_Long 2
#define ButtonPressType_Reset 3
#define ButtonPressInterval_Long 1000      // ms
#define ButtonPressInterval_Reset 3000     // ms
#define ButtonPressInterval_Max 10000      // ms
#define TempSensorStatus_NA 0
#define TempSensorStatus_OK 1
#define TempSensorStatus_NOK -1
#define TempSensorStatus_NotFound -2
#define TempSensorStatus_OutOfAbsLimit -3
#define TempSensorStatus_OutOfOpLimit -4
#define TempSensorAbsMax 125
#define TempSensorAbsMin -55
#define TempSensorOpMax 40
#define TempSensorOpMin 0

// Function Declarations
void SetupButton();          // Sets up the button with debouncing
void ReadButton();           // Reads button state and handles logic
void SetupTempSensor();      // Sets up the DS18B20 temperature sensor
float ReadTempSensor();       // Reads temperature from the DS18B20 sensor

#endif // INPUTS_H
