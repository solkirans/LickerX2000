#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <Arduino.h>

// Global Variables
extern bool HeaterRequest;
extern bool PumpRequest;
extern int8_t PumpStatus;

// Function Declarations
void PumpSetup(void);
void PumpStart(void);
void HeaterSetup(void);
int8_t HeaterStart(float TempTarget_, float TempCurrent_, int8_t TempSensorStatus_);
void LedSetup(void);
void LedSineAnimation(uint16_t Amplitude, uint8_t BPM, uint8_t StateRequested);

// Constant Definitions
#define HeaterPin 16
#define PumpPin 17
#define LedPin 25
#define Pin_Off 0
#define Pin_On 1
#define HeaterTimeout 180000 // ms
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
#define PWM_CHANNEL 0 // PWM channel
#define PWM_RESOLUTION 12 // 12-bit resolution
#define PWM_MAX ((1 << PWM_RESOLUTION) - 1) // Max PWM value
#define PWM_BASE_FREQ 80000000  // ESP32 LEDC clock (80 MHz)
#define PWM_MAX_FREQ (PWM_BASE_FREQ / (1 << PWM_RESOLUTION)) // Max frequency for 12-bit
#define PWM_FREQ (PWM_MAX_FREQ / 2) // Half of max frequency
#define LedAnimationState_NA 0
#define LedAnimationState_Stop 1
#define LedAnimationState_Set 2
#define LedAnimationState_Off 3
#define LedAnimationState_On 4
#define LedAnimationState_Flashing 5
#define LedAnimationState_Sine 6
#define LedAnimationState_Transition 7
#define TransitionTime 500 // ms

#endif
