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
    void updateWorkPositionX(float x);
    void updateWorkPositionY(float y);
    void updateWorkPositionZ(float z);
    void updateFeed(float feed);
    void updateSpeed(float speed);
    void updatePinStateX(bool state);
    void updatePinStateY1(bool state);
    void updatePinStateY2(bool state);
    void updatePinStateZ(bool state);
    void updatePinStateH(bool state);
    void updateWorkCoordOffsetX(float x);
    void updateWorkCoordOffsetY(float y);
    void updateWorkCoordOffsetZ(float z);
    void updateOverrideFeed(int ovFeed);
    void updateOverrideRapid(int ovRapid);
    void updateOverrideSpindle(int ovSpindle);  
    void updateQueueItems(int items);


private:
    BLEServer *server;
    BLEService *service;
    BLECharacteristic *machineState;
    BLECharacteristic *queueItems;
    BLECharacteristic *workPositionX;
    BLECharacteristic *workPositionY;
    BLECharacteristic *workPositionZ;
    BLECharacteristic *feed;
    BLECharacteristic *speed;
    BLECharacteristic *pinStateX;
    BLECharacteristic *pinStateY1;
    BLECharacteristic *pinStateY2;
    BLECharacteristic *pinStateZ;
    BLECharacteristic *pinStateH;
    BLECharacteristic *workCoordOffsetX;
    BLECharacteristic *workCoordOffsetY;
    BLECharacteristic *workCoordOffsetZ;
    BLECharacteristic *overrideFeed;
    BLECharacteristic *overrideRapid;
    BLECharacteristic *overrideSpindle;

    class BLEServerEventHandler : public BLEServerCallbacks
    {
        void onConnect(BLEServer *pServer) override;
        void onDisconnect(BLEServer *pServer) override;
    };
};

extern BLEManager bleManager;

#endif