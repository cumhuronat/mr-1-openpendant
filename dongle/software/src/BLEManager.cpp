#include "BLEManager.h"
#include "USBManager.h"

void BLEManager::begin()
{
    BLEDevice::init(BLE_DEVICE_NAME);

    server = BLEDevice::createServer();
    server->setCallbacks(new BLEServerEventHandler());
    service = server->createService(SERVICE_UUID);
    machineState = service->createCharacteristic(CHARACTERISTIC_MACHINE_STATE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    workPosition = service->createCharacteristic(CHARACTERISTIC_WORK_POSITION_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    feedAndSpeed = service->createCharacteristic(CHARACTERISTIC_FEED_AND_SPEED_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pinState = service->createCharacteristic(CHARACTERISTIC_PIN_STATE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    workCoordOffsets = service->createCharacteristic(CHARACTERISTIC_WORK_COORD_OFFSETS_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    overrides = service->createCharacteristic(CHARACTERISTIC_OVERRIDE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    service->start();
    BLEAdvertising *pAdvertising = server->getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->start();
}

void BLEManager::BLEServerEventHandler::onConnect(BLEServer *pServer)
{
    pServer->startAdvertising();
}

void BLEManager::BLEServerEventHandler::onDisconnect(BLEServer *pServer)
{
    pServer->startAdvertising();
}

void BLEManager::updateMachineState(char status[10])
{
    bleManager.machineState->setValue(status);
    bleManager.machineState->notify();
}