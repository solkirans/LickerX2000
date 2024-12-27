#include "outputs.h"

// Global Variables
bool HeaterRequest = false;
bool PumpRequest = false;
int8_t PumpStatus = PumpStatus_NA;

unsigned long LedStartTime = 0;
unsigned long PumpStartTime = 0;
bool PumpRequest_Prev = false;
bool HeaterRequest_Prev = false;
unsigned long HeaterStartTime = 0;
uint8_t LedStatus = Pin_Off;
uint8_t LedAnimationState = LedAnimationState_NA;
uint8_t LedAnimationState_Prev = LedAnimationState_NA;

void PumpSetup(void) {
    pinMode(PumpPin, OUTPUT);
    digitalWrite(PumpPin, Pin_Off);
    PumpRequest_Prev = false;
}

void PumpStart(void) {
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

void HeaterSetup(void) {
    pinMode(HeaterPin, OUTPUT);
    digitalWrite(HeaterPin, Pin_Off);
    HeaterRequest_Prev = false;
}

int8_t HeaterStart(float TempTarget_, float TempCurrent_, int8_t TempSensorStatus_) {
    if (TempSensorStatus_ != HeaterTempStatus_Ready) return HeaterTempStatus_NA;

    unsigned long interval = millis() - HeaterStartTime;

    if (HeaterRequest && !HeaterRequest_Prev) {
        HeaterStartTime = millis();
        HeaterRequest_Prev = HeaterRequest;
    }

    if (HeaterRequest) {
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
}

void LedSetup(void) {
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(LedPin, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, 0);
}

void LedSineAnimation(uint16_t Amplitude, uint8_t BPM, uint8_t StateRequested) {
    static unsigned long lastUpdate = 0;
    unsigned long currentTime = millis();

    if (StateRequested != LedAnimationState_Prev || LedAnimationState != StateRequested) {
        LedAnimationState_Prev = StateRequested;
        lastUpdate = currentTime;
    }

    switch (StateRequested) {
        case LedAnimationState_Stop:
            ledcWrite(PWM_CHANNEL, 0);
            break;

        case LedAnimationState_Set:
            ledcWrite(PWM_CHANNEL, Amplitude > PWM_MAX ? PWM_MAX : Amplitude);
            break;

        case LedAnimationState_Off:
            ledcWrite(PWM_CHANNEL, 0);
            break;

        case LedAnimationState_On:
            ledcWrite(PWM_CHANNEL, PWM_MAX);
            break;

        case LedAnimationState_Flashing:
            if ((currentTime / (60000 / BPM)) % 2 == 0) {
                ledcWrite(PWM_CHANNEL, Amplitude > PWM_MAX ? PWM_MAX : Amplitude);
            } else {
                ledcWrite(PWM_CHANNEL, 0);
            }
            break;

        case LedAnimationState_Sine: {
            float phase = (currentTime % (60000 / BPM)) / (float)(60000 / BPM);
            float sineValue = 0.5 * (1 + sin(2 * PI * phase - PI / 2));
            uint32_t pwmValue = sineValue * (Amplitude > PWM_MAX ? PWM_MAX : Amplitude);
            ledcWrite(PWM_CHANNEL, pwmValue);
            break;
        }

        case LedAnimationState_Transition: {
            // Smooth transition logic
            break;
        }

        default:
            break;
    }
}
