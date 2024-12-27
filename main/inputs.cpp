#include "inputs.h"

// Global Variable Definitions
int8_t TempSensorStatus = TempSensorStatus_NA;
int8_t ButtonStatus = ButtonStatus_NA;
int8_t ButtonPressType = ButtonPressType_NA;
unsigned long ButtonPressTime = 0;
bool ButtonPressTypeDataReady = false;
bool ButtonPressTypeDataClearReq = false;

// Local Variables
bool ButtonPressTypeDataClearReq_Prev = false;
float TempPrev = 0.0;
Bounce buttonFilter;
OneWire oneWire(TempSensorPin);
DallasTemperature tempSensor(&oneWire);

// Setup Button Function
void SetupButton() {
    pinMode(ButtonPin, INPUT_PULLDOWN);
    buttonFilter.attach(ButtonPin);
    buttonFilter.interval(ButtonDebounceFilterTime);

    ButtonStatus = ButtonStatus_NA;
    ButtonPressType = ButtonPressType_NA;
    ButtonPressTime = 0;
    ButtonPressTypeDataReady = false;
    ButtonPressTypeDataClearReq = false;
    ButtonPressTypeDataClearReq_Prev = false;
    Serial.println("INFO: Button setup completed.");
}

// Read Button Function
void ReadButton() {
    buttonFilter.update();
    if (ButtonPressTypeDataClearReq && !ButtonPressTypeDataClearReq_Prev) {
        ButtonPressTypeDataReady = false;
        ButtonPressTypeDataClearReq_Prev = true;
    } else if (!ButtonPressTypeDataClearReq) {
        ButtonPressTypeDataClearReq_Prev = false;
    }

    if (ButtonStatus == ButtonStatus_Pressed && buttonFilter.read() == LOW) {
        ButtonStatus = ButtonStatus_Released;
        unsigned long elapsedTime = millis() - ButtonPressTime;
        if (elapsedTime >= ButtonPressInterval_Reset && elapsedTime < ButtonPressInterval_Max) {
            ButtonPressType = ButtonPressType_Reset;
        } else if (elapsedTime >= ButtonPressInterval_Long) {
            ButtonPressType = ButtonPressType_Long;
        } else if (elapsedTime <= ButtonPressInterval_Long) {
            ButtonPressType = ButtonPressType_Short;
        }
        ButtonPressTypeDataReady = true;
        ButtonPressTime = millis() - (ButtonPressInterval_Max + 1); // Prevent accidental rechecks
        Serial.println("INFO: Button released.");
    } else if ((ButtonStatus == ButtonStatus_Released || ButtonStatus == ButtonStatus_NA) && buttonFilter.read() == HIGH) {
        ButtonStatus = ButtonStatus_Pressed;
        ButtonPressTime = millis();
        Serial.println("INFO: Button pressed.");
    }
}

// Setup Temp Sensor Function
void SetupTempSensor() {
    tempSensor.begin();
    tempSensor.setResolution(10);
    if (!tempSensor.getDeviceCount()) {
        TempSensorStatus = TempSensorStatus_NotFound;
        Serial.println("ERROR: No temperature sensor found.");
    } else {
        TempSensorStatus = TempSensorStatus_OK;
        Serial.println("INFO: Temperature sensor setup completed.");
    }
}

// Read Temp Sensor Function
float ReadTempSensor() {
    tempSensor.requestTemperatures();
    float TempCurrentLocal = tempSensor.getTempCByIndex(0);
    if (TempCurrentLocal < TempSensorAbsMin || TempCurrentLocal > TempSensorAbsMax) {
        TempSensorStatus = TempSensorStatus_OutOfAbsLimit;
        Serial.print("WARNING: Temperature out of absolute limits:");
        Serial.println(TempCurrentLocal);
    } else if (TempCurrentLocal < TempSensorOpMin || TempCurrentLocal > TempSensorOpMax) {
        TempSensorStatus = TempSensorStatus_OutOfOpLimit;
        Serial.print("WARNING: Temperature out of operational limits:");
        Serial.println(TempCurrentLocal);
    } else if (abs(TempCurrentLocal - TempPrev) > (TempSensorOpMax - TempSensorOpMin)) {
        TempSensorStatus = TempSensorStatus_NOK;
        Serial.println("WARNING: Temperature deviation too high.");
        TempPrev = TempCurrentLocal;
    } else {
        TempPrev = TempCurrentLocal;
        TempSensorStatus = TempSensorStatus_OK;
    }
    return TempCurrentLocal;
}
