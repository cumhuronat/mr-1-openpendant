#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "Config.h"

class BLEManager
{
public:
    void begin();

private:
    BLEServer *pServer;
    BLEService *pService;
    BLECharacteristic *pCharacteristic;

    class MyServerCallbacks : public BLEServerCallbacks
    {
        void onConnect(BLEServer *pServer) override;
        void onDisconnect(BLEServer *pServer) override;
    };

    class MyCharacteristicCallbacks : public BLECharacteristicCallbacks
    {
        void onWrite(BLECharacteristic *pCharacteristic) override;
    };
};

extern BLEManager bleManager;

#endif