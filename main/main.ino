#include "VL53L0XSensor.h"

void setup() {
    Serial.begin(115200);
    delay(100);

    int initStatus = VL53L0X_SensorInit();
    if (initStatus < 0) {
        Serial.print("[ERROR] Sensor initialization failed with error code: ");
        Serial.println(initStatus);
        while (1); // Halt if initialization fails
    }

    Serial.println("[INFO] All sensors initialized successfully.");
}

void loop() {
    static unsigned long lastMeasurementTime = 0;
    static unsigned long lastAveragePrintTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastMeasurementTime >= 100) {
        lastMeasurementTime = currentTime;
        VL53L0X_ReadMeasurement();
    }

    if (currentTime - lastAveragePrintTime >= 500) {
        lastAveragePrintTime = currentTime;

        for (uint8_t i = 0; i < NUM_SENSORS; i++) {
            float average = VL53L0X_GetAverage(i);
            if (average < 0) {
                Serial.print("[ERROR] Failed to calculate average for sensor ");
                Serial.print(i);
                Serial.println("!");
            } else {
                Serial.print("[INFO] Sensor ");
                Serial.print(i);
                Serial.print(" average distance: ");
                Serial.print(average);
                Serial.println(" mm");
            }
        }
    }
}
