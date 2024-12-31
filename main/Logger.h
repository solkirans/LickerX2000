#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

// BLE UUIDs
#define BLE_SERVICE_UUID "00001800-0000-1000-8000-00805f9b34fb"
#define BLE_CHARACTERISTIC_UUID "00002a00-0000-1000-8000-00805f9b34fb"

// Logger Parameters
#define Logger_BLE_Enabled 1
#define Logger_Serial_Enabled 0
#define PrintText_SerialFail -1
#define PrintText_BleFail -2
#define PrintText_BleAndSerialFail -3
#define PrintText_NA 0
#define PrintText_Success 1
#define LoggerDebugLevel 1
#define LoggerDebugLevelDefault 1
#define LoggerDebugLevel_Error 4
#define LoggerDebugLevel_Warning 3
#define LoggerDebugLevel_Info 2
#define LoggerDebugLevel_Debug 1
#define LoggerDebugLevel_NA 0

// Extern variables
extern bool bleConnected;
extern BLECharacteristic *pCharacteristic;

// Function prototypes
void SetupLogger(void);
void SetupBLE(void);
void SetupSerial(void);
int PrintText(int DebugLevel, String OutputText);

#endif // LOGGER_H
