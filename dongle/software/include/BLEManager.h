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
    void updateMachineState(char status[10]);

private:
    BLEServer *server;
    BLEService *service;
    BLECharacteristic *machineState;
    BLECharacteristic *workPosition;
    BLECharacteristic *feedAndSpeed;
    BLECharacteristic *pinState;
    BLECharacteristic *workCoordOffsets;
    BLECharacteristic *overrides;

    class BLEServerEventHandler : public BLEServerCallbacks
    {
        void onConnect(BLEServer *pServer) override;
        void onDisconnect(BLEServer *pServer) override;
    };
};

extern BLEManager bleManager;

#endif