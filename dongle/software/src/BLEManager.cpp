#include "BLEManager.h"
#include "USBManager.h"

void BLEManager::begin()
{
    BLEDevice::init(BLE_DEVICE_NAME);

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE);

    pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->start();
}

void BLEManager::MyServerCallbacks::onConnect(BLEServer *pServer)
{
    pServer->startAdvertising();
}

void BLEManager::MyServerCallbacks::onDisconnect(BLEServer *pServer)
{
    pServer->startAdvertising();
}

void BLEManager::MyCharacteristicCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    String value = pCharacteristic->getValue().c_str();
    if (value.length() > 0)
    {
        USBManager::getSerialHost().println(value);
        pCharacteristic->setValue("");
    }
}