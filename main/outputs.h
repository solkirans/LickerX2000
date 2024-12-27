#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <Arduino.h>

// Global Variable Definitions
extern bool HeaterRequest;
extern bool PumpRequest;
extern int8_t PumpStatus;

// Constant Definitions
#define HeaterPin 16
#define PumpPin 17
#define LedPin 25
#define Pin_Off 0
#define Pin_On 1
#define HeaterTimeout 120000 // ms
#define HeaterTempStatus_NA 0
#define HeaterTempStatus_Ready 1
#define HeaterTempStatus_Off 2
#define HeaterTempStatus_Ongoing 3
#define HeaterTempStatus_Timeout -1
#define PumpTimeout 60000 // ms
#define PumpStatus_NA 0
#define PumpStatus_Off 1
#define PumpStatus_On 2
#define PumpStatus_Timeout 3
#define LedFlashInterval 500 // ms

// Function Declarations
void PumpSetup(void);
void PumpStart(void);
void HeaterSetup(void);
int8_t HeaterStart(float TempTarget_, float TempCurrent_, int8_t TempSensorStatus_);
void LedSetup(void);
void LedFlash(void);
void LedSet(uint8_t State);
void LedTempAnimation(float TempCurrent_, float TempSensorOpMin_, float TempSensorOpMax_);

#endif // OUTPUTS_H
