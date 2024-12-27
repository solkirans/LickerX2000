#include "inputs.h"

// Global Variables
int8_t TempSensorStatus = TempSensorStatus_NA;
float TempPrev = 0;

Bounce buttonFilter;
OneWire oneWire(TempSensorPin);
DallasTemperature tempSensor(&oneWire);

void SetupButton() {
    pinMode(ButtonPin, INPUT_PULLDOWN);
    buttonFilter.attach(ButtonPin);
    buttonFilter.interval(ButtonDebounceFilterTime);
}

bool ReadButton() {
    buttonFilter.update();
    bool roseEvent = buttonFilter.rose();
    if (roseEvent==true)
    {
      Serial.println("Button Rise");
    }
    return roseEvent;
}

void SetupTempSensor() {
    tempSensor.begin();
    tempSensor.setResolution(10); // 10-bit resolution

    if (tempSensor.getDeviceCount() == 0) {
        TempSensorStatus = TempSensorStatus_NotFound;
    }
}

float ReadTempSensor() {
    tempSensor.requestTemperatures();
    float TempCurrentLocal = tempSensor.getTempCByIndex(0);

    if (TempCurrentLocal < TempSensorAbsMin || TempCurrentLocal > TempSensorAbsMax) {
        TempSensorStatus = TempSensorStatus_OutOfAbsLimit;
    } else if (TempCurrentLocal < TempSensorOpMin || TempCurrentLocal > TempSensorOpMax) {
        TempSensorStatus = TempSensorStatus_OutOfOpLimit;
    } else {
        TempPrev = TempCurrentLocal;
        TempSensorStatus = TempSensorStatus_OK;
    }

    return TempCurrentLocal;
}
