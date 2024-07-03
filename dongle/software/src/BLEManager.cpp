#include "BLEManager.h"
#include "USBManager.h"

void BLEManager::begin()
{
    BLEDevice::init(BLE_DEVICE_NAME);

    server = BLEDevice::createServer();
    server->setCallbacks(new BLEServerEventHandler());
    service = server->createService(SERVICE_UUID);
    machineState = service->createCharacteristic(CHARACTERISTIC_MACHINE_STATE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    queueItems = service->createCharacteristic(CHARACTERISTIC_QUEUE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    workPositionX = service->createCharacteristic(CHARACTERISTIC_WORK_POSITION_X_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    workPositionY = service->createCharacteristic(CHARACTERISTIC_WORK_POSITION_Y_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    workPositionZ = service->createCharacteristic(CHARACTERISTIC_WORK_POSITION_Z_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    feed = service->createCharacteristic(CHARACTERISTIC_FEED_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    speed = service->createCharacteristic(CHARACTERISTIC_SPEED_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pinStateX = service->createCharacteristic(CHARACTERISTIC_PIN_STATE_X_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pinStateY1 = service->createCharacteristic(CHARACTERISTIC_PIN_STATE_Y1_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pinStateY2 = service->createCharacteristic(CHARACTERISTIC_PIN_STATE_Y2_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pinStateZ = service->createCharacteristic(CHARACTERISTIC_PIN_STATE_Z_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pinStateH = service->createCharacteristic(CHARACTERISTIC_PIN_STATE_H_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    workCoordOffsetX = service->createCharacteristic(CHARACTERISTIC_WORK_COORD_OFFSET_X_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    workCoordOffsetY = service->createCharacteristic(CHARACTERISTIC_WORK_COORD_OFFSET_Y_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    workCoordOffsetZ = service->createCharacteristic(CHARACTERISTIC_WORK_COORD_OFFSET_Z_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    overrideFeed = service->createCharacteristic(CHARACTERISTIC_OVERRIDE_FEED_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    overrideRapid = service->createCharacteristic(CHARACTERISTIC_OVERRIDE_RAPID_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    overrideSpindle = service->createCharacteristic(CHARACTERISTIC_OVERRIDE_SPINDLE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

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

void BLEManager::updateWorkPositionX(float x)
{
    bleManager.workPositionX->setValue(x);
    bleManager.workPositionX->notify();
}

void BLEManager::updateWorkPositionY(float y)
{
    bleManager.workPositionY->setValue(y);
    bleManager.workPositionY->notify();
}

void BLEManager::updateWorkPositionZ(float z)
{
    bleManager.workPositionZ->setValue(z);
    bleManager.workPositionZ->notify();
}

void BLEManager::updateFeed(float feed)
{
    bleManager.feed->setValue(feed);
    bleManager.feed->notify();
}

void BLEManager::updateSpeed(float speed)
{
    bleManager.speed->setValue(speed);
    bleManager.speed->notify();
}

void BLEManager::updatePinStateX(bool state)
{
    int intValue = static_cast<int>(state);
    bleManager.pinStateX->setValue(intValue);
    bleManager.pinStateX->notify();
}

void BLEManager::updatePinStateY1(bool state)
{
    int intValue = static_cast<int>(state);
    bleManager.pinStateY1->setValue(intValue);
    bleManager.pinStateY1->notify();
}

void BLEManager::updatePinStateY2(bool state)
{
    int intValue = static_cast<int>(state);
    bleManager.pinStateY2->setValue(intValue);
    bleManager.pinStateY2->notify();
}

void BLEManager::updatePinStateZ(bool state)
{
    int intValue = static_cast<int>(state);
    bleManager.pinStateZ->setValue(intValue);
    bleManager.pinStateZ->notify();
}

void BLEManager::updatePinStateH(bool state)
{
    int intValue = static_cast<int>(state);
    bleManager.pinStateH->setValue(intValue);
    bleManager.pinStateH->notify();
}

void BLEManager::updateWorkCoordOffsetX(float x)
{
    bleManager.workCoordOffsetX->setValue(x);
    bleManager.workCoordOffsetX->notify();
}

void BLEManager::updateWorkCoordOffsetY(float y)
{
    bleManager.workCoordOffsetY->setValue(y);
    bleManager.workCoordOffsetY->notify();
}

void BLEManager::updateWorkCoordOffsetZ(float z)
{
    bleManager.workCoordOffsetZ->setValue(z);
    bleManager.workCoordOffsetZ->notify();
}

void BLEManager::updateOverrideFeed(int ovFeed)
{
    bleManager.overrideFeed->setValue(ovFeed);
    bleManager.overrideFeed->notify();
}

void BLEManager::updateOverrideRapid(int ovRapid)
{
    bleManager.overrideRapid->setValue(ovRapid);
    bleManager.overrideRapid->notify();
}

void BLEManager::updateOverrideSpindle(int ovSpindle)
{
    bleManager.overrideSpindle->setValue(ovSpindle);
    bleManager.overrideSpindle->notify();
}

void BLEManager::updateQueueItems(int items)
{
    bleManager.queueItems->setValue(items);
    bleManager.queueItems->notify();
}
