#include <Arduino.h>
#include "inputs.h"
#include "outputs.h"

// Parameters
#define TempTarget 37
#define State_NA 0
#define State_Init 1
#define State_Idle 2
#define State_AwaitingHeating 3
#define State_Heating 4
#define State_HeatingDone 5
#define State_HeatingSkipped 6
#define State_AwaitingPumpReq 7
#define State_AwaitingPumping 8
#define State_Pumping 9
#define State_AwaitingPumpStop 10
#define State_PumpingDone 11

// Variables
uint8_t StateCurrent = State_NA;
uint8_t StatePrev = State_NA;

bool SerialEnable = false;

// Helper Function to Convert State and Status to String
String getStateName(uint8_t state) {
    switch (state) {
        case State_NA: return "NA";
        case State_Init: return "Init";
        case State_Idle: return "Idle";
        case State_AwaitingHeating: return "Awaiting Heating";
        case State_Heating: return "Heating";
        case State_HeatingDone: return "Heating Done";
        case State_HeatingSkipped: return "Heating Skipped";
        case State_AwaitingPumpReq: return "Awaiting Pump Req";
        case State_AwaitingPumping: return "Awaiting Pumping";
        case State_Pumping: return "Pumping";
        case State_AwaitingPumpStop: return "Awaiting Pump Stop";
        case State_PumpingDone: return "Pumping Done";
        default: return "Unknown";
    }
}

String getHeaterStatusName(int8_t status) {
    switch (status) {
        case HeaterTempStatus_NA: return "NA";
        case HeaterTempStatus_Ready: return "Ready";
        case HeaterTempStatus_Off: return "Off";
        case HeaterTempStatus_Ongoing: return "Ongoing";
        case HeaterTempStatus_Timeout: return "Timeout";
        default: return "Unknown";
    }
}

String getPumpStatusName(int8_t status) {
    switch (status) {
        case PumpStatus_NA: return "NA";
        case PumpStatus_Off: return "Off";
        case PumpStatus_On: return "On";
        case PumpStatus_Timeout: return "Timeout";
        default: return "Unknown";
    }
}

void setup() {
    // Initialize serial port
    Serial.begin(115200);

    // Initialize state and requests
    StateCurrent = State_Init;
    StatePrev = State_Init;
    HeaterRequest = false;
    PumpRequest = false;

    // Setup components
    SetupButton();
    SetupTempSensor();
    PumpSetup();
    HeaterSetup();
    LedSetup();

    // Check if TempSensor setup returned an error
    if (TempSensorStatus < 0) {
        Serial.println("[ERROR] Temperature Sensor Initialization Failed!");
        while (1); // Halt execution
    }
}

void loop() {
    // Save previous state
    StatePrev = StateCurrent;

    // Read current system states
    int PumpPinState = digitalRead(PumpPin);
    int HeaterPinState = digitalRead(HeaterPin);
    float TempCurrent = ReadTempSensor();
    int8_t heaterStatus = HeaterStart(TempTarget, TempCurrent , TempSensorStatus);
    bool ButtonState = ReadButton(); 
    PumpStart();
    if (SerialEnable)
    {
      // Debug Outputs
      Serial.print("[INFO] Temperature: ");
      Serial.print(TempCurrent);
      Serial.print(" PumpPin: ");
      Serial.print(PumpPinState);
      Serial.print(" HeaterPin: ");
      Serial.println(HeaterPinState);
      Serial.print("[INFO] StateCurrent: ");
      Serial.print(getStateName(StateCurrent));
      Serial.print(" Pump Status: ");
      Serial.println(getPumpStatusName(PumpStatus));
      Serial.print(" Heater Status: ");
      Serial.println(getHeaterStatusName(heaterStatus));
      delay(50);
    }
    // State Machine Logic
    switch (StateCurrent) {
        case State_Init:
            LedSineAnimation(4096, 60, LedAnimationState_Off);
            StateCurrent = State_Idle;
            break;

        case State_Idle:
            LedSineAnimation(4096, 60, LedAnimationState_On);
            if (ButtonState) {
                HeaterRequest = true;
                StateCurrent = State_AwaitingHeating;
            }
            break;

        case State_AwaitingHeating:
            LedSineAnimation(4096, 60, LedAnimationState_On);
            if (heaterStatus == HeaterTempStatus_Ongoing) {
                StateCurrent = State_Heating;
            } else if (heaterStatus == HeaterTempStatus_Ready) {
                StateCurrent = State_HeatingDone;
            }
            break;

        case State_Heating:
            LedSineAnimation(4096, 30, LedAnimationState_Sine);
            if ((heaterStatus == HeaterTempStatus_Timeout) || (heaterStatus == HeaterTempStatus_Ready)||(ButtonState)) {
                StateCurrent = State_HeatingDone;
                HeaterRequest = false;
            }
            break;

        case State_HeatingDone:
            LedSineAnimation(4096, 60, LedAnimationState_On);
            StateCurrent = State_AwaitingPumpReq;
            HeaterRequest = false;
            break;

        case State_AwaitingPumpReq:
            LedSineAnimation(4096, 120, LedAnimationState_Flashing);
            if (ButtonState) {
                PumpRequest = true;
                StateCurrent = State_AwaitingPumping;
            }
            break;

        case State_AwaitingPumping:
            LedSineAnimation(4096, 120, LedAnimationState_Flashing);
            if (PumpStatus == PumpStatus_On) {
                StateCurrent = State_Pumping;
            }
            break;

        case State_Pumping:
            LedSineAnimation(4096, 60, LedAnimationState_Sine);
            if ((ButtonState) || (PumpStatus == PumpStatus_Timeout)) {
                PumpRequest = false;
                StateCurrent = State_AwaitingPumpStop;
            }
            break;

        case State_AwaitingPumpStop:
            LedSineAnimation(4096, 90, LedAnimationState_Sine);
            if (PumpStatus == PumpStatus_Off || PumpStatus == PumpStatus_Timeout) {
                StateCurrent = State_PumpingDone;
            }
            break;

        case State_PumpingDone:
            PumpRequest = false;
            HeaterRequest = false;
            StateCurrent = State_Idle;
            break;

        default:
            StateCurrent = State_NA;
            break;
    }
}
