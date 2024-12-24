#include "VL53L0XSensor.h"

// Global variables
VL53L0X VL53L0X_sensors[NUM_SENSORS];
uint8_t VL53L0X_shutdownPins[NUM_SENSORS] = {26};
uint8_t VL53L0X_addresses[NUM_SENSORS] = {0x29};
uint16_t VL53L0X_measurementArrays[NUM_SENSORS][VL53L0X_MEASUREMENT_ARRAY_SIZE] = {{0}};
uint8_t VL53L0X_measurementIndices[NUM_SENSORS] = {0};

void VL53L0X_CheckOutputPins()
{
      for (uint8_t i = 0; i < NUM_SENSORS; i++) {
        uint8_t pinState = digitalRead(VL53L0X_shutdownPins[i]);
        Serial.print("Pin ");
        Serial.print(VL53L0X_shutdownPins[i]);
        Serial.print(": ");
        Serial.println(pinState == HIGH ? "HIGH" : "LOW");
    }
}

void VL53L0X_i2cScan()
{

    // Scan I2C bus and print device addresses
    Serial.println("[INFO] Scanning I2C bus...");
    for (uint8_t address = 1; address < 127; address++) {
        //Serial.print("Address:");
        //Serial.println(address, HEX);
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();
        if (error == 0) {
            Serial.print("[INFO] Device found at address 0x");
            Serial.println(address, HEX);
        } 
        else if (error == 4){
            Serial.print("[ERROR] Address 0x");
            Serial.print(address, HEX);
            Serial.print("response:");
            Serial.println(error);
        }
    }
    Serial.println("[INFO] Scanning done.");
}
int VL53L0X_SetupSensors() {
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
        pinMode(VL53L0X_shutdownPins[i], OUTPUT);
        digitalWrite(VL53L0X_shutdownPins[i], LOW);
    }
    delay(100);

    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
        digitalWrite(VL53L0X_shutdownPins[i], HIGH);
        delay(100);

        if (!VL53L0X_sensors[i].init()) {
            Serial.print("[ERROR] Failed to initialize sensor ");
            Serial.println(i);
            return VL53L0X_ERR_SETUP_FAIL;
        }

        VL53L0X_sensors[i].setAddress(VL53L0X_addresses[i]);

        Wire.beginTransmission(VL53L0X_addresses[i]);
        if (Wire.endTransmission() != 0) {
            Serial.print("[ERROR] Failed to detect sensor at address 0x");
            Serial.println(VL53L0X_addresses[i], HEX);
            return VL53L0X_ERR_I2C_SCAN_MISSING;
        }

        Serial.print("[INFO] Sensor ");
        Serial.print(i);
        Serial.print(" initialized and address set. to ");
        Serial.println(VL53L0X_addresses[i], HEX);
        VL53L0X_CheckOutputPins();
        VL53L0X_i2cScan(); 
        digitalWrite(VL53L0X_shutdownPins[i], LOW);
        delay(100);  
    }
    
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
        digitalWrite(VL53L0X_shutdownPins[i], HIGH);
        delay(100);
    }
    delay(500);
    VL53L0X_CheckOutputPins();
    VL53L0X_i2cScan();
    

    return 0;
}

int VL53L0X_SensorInit() {
    Wire.begin(I2C_SDA, I2C_SCL);
    delay(10);

    int status = VL53L0X_SetupSensors();
    if (status < 0) {
        return status;
    }
    Serial.println("[INFO] All sensors setup successfully.");
    status = VL53L0X_Calibrate();
    if (status < 0) {
        return status;
    }
    Serial.println("[INFO] All sensors calibrated successfully.");
    return 0;
}

int VL53L0X_Calibrate() {
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
        VL53L0X_sensors[i].setSignalRateLimit(0.1);
        Serial.println("[INFO] Calibrated: setSignalRateLimit");
        VL53L0X_sensors[i].setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
        Serial.println("[INFO] Calibrated: VcselPeriodPreRange");
        VL53L0X_sensors[i].setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
        Serial.println("[INFO] Calibrated: VcselPeriodFinalRange");
        VL53L0X_sensors[i].setMeasurementTimingBudget(200000);
        Serial.println("[INFO] Calibrated: setMeasurementTimingBudget");
        if (!VL53L0X_sensors[i].setMeasurementTimingBudget(200000)) {
            Serial.print("[ERROR] Calibration failed for sensor ");
            Serial.println(i);
            return VL53L0X_ERR_CALIB_FAIL;
        }
    }
    return 0;
}

int VL53L0X_ReadMeasurement() {
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
        uint16_t distance = VL53L0X_sensors[i].readRangeSingleMillimeters();

        if (VL53L0X_sensors[i].timeoutOccurred()) {
            Serial.print("[ERROR] Timeout occurred for sensor ");
            Serial.println(i);
            return VL53L0X_ERR_MEASUREMENT_FAIL;
        }

        if (distance < VL53L0X_MIN_DISTANCE) {
            distance = VL53L0X_MIN_DISTANCE;
        } else if (distance > VL53L0X_MAX_DISTANCE) {
            distance = VL53L0X_MAX_DISTANCE;
        }

        VL53L0X_measurementArrays[i][VL53L0X_measurementIndices[i]] = distance;
        VL53L0X_measurementIndices[i] = (VL53L0X_measurementIndices[i] + 1) % VL53L0X_MEASUREMENT_ARRAY_SIZE;

        Serial.print("[INFO] Sensor ");
        Serial.print(i);
        Serial.print(" distance: ");
        Serial.print(distance);
        Serial.println(" mm");
    }

    return 0;
}

float VL53L0X_GetAverage(uint8_t sensorIndex) {
    uint32_t sum = 0;
    uint8_t validCount = 0;

    for (uint8_t i = 0; i < VL53L0X_MEASUREMENT_ARRAY_SIZE; i++) {
        if (VL53L0X_measurementArrays[sensorIndex][i] > 0) {
            sum += VL53L0X_measurementArrays[sensorIndex][i];
            validCount++;
        }
    }

    if (validCount == 0) {
        return VL53L0X_ERR_NO_VALID_DATA;
    }

    return (float)sum / validCount;
}
