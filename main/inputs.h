#ifndef INPUTS_H
#define INPUTS_H

#include <Bounce2.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Constants
#define TempSensorPin 32
#define ButtonPin 33
#define ButtonDebounceFilterTime 100 // ms
#define TempSensorStatus_NA 0
#define TempSensorStatus_OK 1
#define TempSensorStatus_NOK -1
#define TempSensorStatus_NotFound -2
#define TempSensorStatus_OutOfAbsLimit -3
#define TempSensorStatus_OutOfOpLimit -4
#define TempSensorAbsMax 125
#define TempSensorAbsMin -55
#define TempSensorOpMax 50
#define TempSensorOpMin 15

// Global Variables
extern int8_t TempSensorStatus;

// Function Declarations
void SetupButton();
bool ReadButton();
void SetupTempSensor();
float ReadTempSensor();

#endif
