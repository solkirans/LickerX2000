#include "Logger.h"

bool bleConnected = false;
BLECharacteristic *pCharacteristic = nullptr;

class BLECallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        bleConnected = true;
    }
    void onDisconnect(BLEServer *pServer) {
        bleConnected = false;
    }
};

void SetupLogger(void) {
    if (Logger_BLE_Enabled) {
        SetupBLE();
    }
    if (Logger_Serial_Enabled) {
        SetupSerial();
    }
}

void SetupBLE(void) {
    BLEDevice::init("AssLickerX2000");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new BLECallbacks());

    BLEService *pService = pServer->createService(BLE_SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        BLE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();
    pServer->getAdvertising()->start();
}

void SetupSerial(void) {
    Serial.begin(115200);
}

int PrintText(int DebugLevel, String OutputText) {
    int returnCode = PrintText_NA;

    if (DebugLevel < LoggerDebugLevel) {
        return returnCode;
    }

    if (Logger_BLE_Enabled && bleConnected) {
        if (pCharacteristic) {
            pCharacteristic->setValue(OutputText.c_str());
            pCharacteristic->notify();
            returnCode = PrintText_Success;
        } else {
            returnCode = PrintText_BleFail;
        }
    }

    if (Logger_Serial_Enabled) {
        if (Serial) {
            Serial.println(OutputText);
            if (returnCode == PrintText_BleFail) {
                returnCode = PrintText_BleAndSerialFail;
            } else {
                returnCode = PrintText_Success;
            }
        } else {
            if (returnCode == PrintText_BleFail) {
                returnCode = PrintText_BleAndSerialFail;
            } else {
                returnCode = PrintText_SerialFail;
            }
        }
    }

    return returnCode;
}
