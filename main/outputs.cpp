#include "outputs.h"

// Global Variable Definitions
bool HeaterRequest = false;
bool PumpRequest = false;
int8_t PumpStatus = PumpStatus_NA;

// Local Variables
unsigned long LedStartTime = 0;
unsigned long PumpStartTime = 0;
bool PumpRequest_Prev = false;
bool HeaterRequest_Prev = false;
unsigned long HeaterStartTime = 0;
uint8_t LedStatus = 0;

// PumpSetup Function
void PumpSetup(void) {
    pinMode(PumpPin, OUTPUT);
    digitalWrite(PumpPin, Pin_Off);
    PumpRequest_Prev = false;
    Serial.println("INFO: Pump setup completed.");
}

// PumpStart Function
void PumpStart() {
    unsigned long interval = millis() - PumpStartTime;

    if (PumpRequest && !PumpRequest_Prev) {
        PumpStartTime = millis();
        PumpRequest_Prev = PumpRequest;
    }

    if (PumpRequest) {
        if (interval < PumpTimeout) {
            digitalWrite(PumpPin, Pin_On);
            PumpStatus = PumpStatus_On;
        } else {
            digitalWrite(PumpPin, Pin_Off);
            PumpStatus = PumpStatus_Timeout;
        }
    } else {
        digitalWrite(PumpPin, Pin_Off);
        PumpStatus = PumpStatus_Off;
        PumpRequest_Prev = false;
    }
}

// HeaterSetup Function
void HeaterSetup(void) {
    pinMode(HeaterPin, OUTPUT);
    digitalWrite(HeaterPin, Pin_Off);
    HeaterRequest_Prev = false;
    Serial.println("INFO: Heater setup completed.");
}

// HeaterStart Function
int8_t HeaterStart(float TempTarget_, float TempCurrent_, int8_t TempSensorStatus_) {
    unsigned long interval = millis() - HeaterStartTime;

    if (TempSensorStatus_ != 1)
    {
      return HeaterTempStatus_NA;
    }

    if (HeaterRequest && !HeaterRequest_Prev) {
        HeaterStartTime = millis();
        HeaterRequest_Prev = HeaterRequest;
    }

    if (HeaterRequest) {
        Serial.print("TempCurrent:");
        Serial.println(TempCurrent_);
        if (interval < HeaterTimeout) {
            if (TempCurrent_ < TempTarget_) {
                digitalWrite(HeaterPin, Pin_On);
                return HeaterTempStatus_Ongoing;
            } else {
                digitalWrite(HeaterPin, Pin_Off);
                return HeaterTempStatus_Ready;
            }
        } else {
            digitalWrite(HeaterPin, Pin_Off);
            return HeaterTempStatus_Timeout;
        }
    } else {
        digitalWrite(HeaterPin, Pin_Off);
        HeaterRequest_Prev = false;
        return HeaterTempStatus_Off;
    }

    return HeaterTempStatus_NA;
}

// LedSetup Function
void LedSetup(void) {
    pinMode(LedPin, OUTPUT);
    digitalWrite(LedPin, Pin_Off);
    Serial.println("INFO: LED setup completed.");
}

// LedFlash Function
void LedFlash(void) {
    unsigned long interval = millis() - LedStartTime;

    if (interval > LedFlashInterval) {
        if (LedStatus == 0) {
            LedSet(100);
        } else {
            LedSet(0);
        }
        LedStartTime = millis();
    }
}

// LedSet Function
void LedSet(uint8_t State) {
    uint8_t StateLocal = (State > 100) ? 100 : State;
    analogWrite(LedPin, StateLocal * 2.55); // Scale 0-100 to 0-255
    LedStatus = StateLocal;
}

// LedTempAnimation Function
void LedTempAnimation(float TempCurrent_, float TempSensorOpMin_, float TempSensorOpMax_) {
    uint8_t RequestedPWMPercentage = (uint8_t)(((TempCurrent_ - TempSensorOpMin_) / (TempSensorOpMax_ - TempSensorOpMin_)) * 100);
    LedSet(RequestedPWMPercentage);
}
