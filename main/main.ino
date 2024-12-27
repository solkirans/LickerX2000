#include <Arduino.h>
#include "inputs.h"
#include "outputs.h"

// Parameters
#define TempTarget 37
#define State_NA 0
#define State_Init 1
#define State_Idle 2
#define State_Heating 3
#define State_AwaitingPumpReq 4
#define State_Pumping 5
#define State_Finished 6

// Variables
uint8_t StateCurrent = State_Init;
uint8_t StatePrev = State_Init;

void setup() {
    // Initialize Serial port
    Serial.begin(115200);
    Serial.println("INFO: System setup started.");

    // Initialize states
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

    // Check for errors during initialization
    if (TempSensorStatus < 0 ) {
        Serial.println("ERROR: Temperature Sensor Initialization failed. Stopping execution.");
        while (1);
    }

    Serial.println("INFO: System setup completed.");
}

void loop() {
    // Save previous state
    StatePrev = StateCurrent;

    // Read temperature and start heater/pump
    float TempCurrent = ReadTempSensor();
    int8_t HeaterStatus = HeaterStart(TempTarget, TempCurrent, TempSensorStatus);
    PumpStart();

    // Debug outputs
    Serial.print("DEBUG: Temp: ");
    Serial.print(TempCurrent);
    Serial.print(" °C, Pump Status: ");
    Serial.print(PumpStatus);
    Serial.print(", Heater Status: ");
    Serial.print(HeaterStatus);
    Serial.print(", State: ");
    Serial.println(StateCurrent);

    delay(50); // Small delay to stabilize system

    // State machine logic
    switch (StateCurrent) {
        case State_Init:
            LedSet(0);
            StateCurrent = State_Idle;
            break;

        case State_Idle:
            LedSet(100);
            ReadButton();
            if (ButtonStatus == ButtonStatus_Pressed) {
                HeaterRequest = true;
                StateCurrent = State_Heating;
            }
            if (HeaterStatus == HeaterTempStatus_Ongoing) {
                StateCurrent = State_Heating;
            }
            if (HeaterStatus == HeaterTempStatus_Ready) {
                StateCurrent = State_AwaitingPumpReq;
            }
            break;

        case State_Heating:
            LedFlash();
            if (HeaterStatus == HeaterTempStatus_Timeout || HeaterStatus == HeaterTempStatus_Ready) {
                StateCurrent = State_AwaitingPumpReq;
                HeaterRequest = false;
            }
            break;

        case State_AwaitingPumpReq:
            LedSet(100);
            ReadButton();
            if (ButtonStatus == ButtonStatus_Pressed) {
                PumpRequest = true;
            }
            if (PumpStatus == PumpStatus_On) {
                StateCurrent = State_Pumping;
            }
            break;

        case State_Pumping:
            LedFlash();
            ReadButton();
            if (ButtonStatus == ButtonStatus_Pressed) {
                PumpRequest = false;
            }
            if (PumpStatus == PumpStatus_Off || PumpStatus == PumpStatus_Timeout) {
                StateCurrent = State_Finished;
            }
            break;

        case State_Finished:
            PumpRequest = false;
            HeaterRequest = false;
            StateCurrent = State_Idle;
            break;

        default:
            Serial.println("ERROR: Unknown state. Resetting to IDLE.");
            StateCurrent = State_Idle;
            break;
    }
}
