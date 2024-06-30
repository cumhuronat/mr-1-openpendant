#include "Adafruit_TinyUSB.h"
#include "SPI.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-cba987654321"

Adafruit_USBH_Host USBHost(&SPI, 38, 35);
Adafruit_USBH_CDC SerialHost;
BLECharacteristic *pCharacteristic;
SemaphoreHandle_t xSemaphore;

bool mr1Ready = false;

void forward_serial_task(void *param) {
  (void)param;
  while (1) {
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
      while (Serial.available()) {
        size_t count = Serial.available();
        uint8_t bufr[count];
        count = Serial.read(bufr, count);

        if (mr1Ready && SerialHost && SerialHost.availableForWrite()) {
          SerialHost.write(bufr, count);
          SerialHost.flush();
        }
      }
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}


void forward_hostserial_task(void *param) {
  (void)param;
  while (1) {
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
      while (SerialHost.connected() && SerialHost.available()) {
        size_t count = SerialHost.available();
        uint8_t bufw[count];
        count = SerialHost.read(bufw, count);
        if (count > 0 && Serial.availableForWrite()) {
          Serial.write(bufw, count);
          Serial.flush();
          mr1Ready = true;
        }
      }
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void usbhost_rtos_task(void *param) {
  (void)param;
  while (1) {
    USBHost.task();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_USB_CDC_EVENTS) {
    arduino_usb_cdc_event_data_t *data = (arduino_usb_cdc_event_data_t *)event_data;
    if (event_id == ARDUINO_USB_CDC_LINE_STATE_EVENT && data->line_state.dtr == 1 && data->line_state.rts == 1) {
      SerialHost.write(0x18);
      SerialHost.flush();
      mr1Ready = false;
    }
  }
}

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue().c_str();
    if (value.length() > 0) {
      SerialHost.println(value);
      pCharacteristic->setValue("");
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    pServer->startAdvertising();  // restart advertising
  };

  void onDisconnect(BLEServer *pServer) {
    pServer->startAdvertising();  // restart advertising
  }
};

void setBle() {
  BLEDevice::init("OpenPendant");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->start();
}

void setup() {
  pinMode(42, OUTPUT);
  digitalWrite(42, HIGH);
  USBHost.begin(1);
  SerialHost.begin(115200);
  Serial.enableReboot(false);
  Serial.begin(115200);
  Serial.onEvent(usbEventCallback);
  setBle();

  xSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(xSemaphore); // Release the semaphore initially

  xTaskCreate(usbhost_rtos_task, "usbh", 2048, NULL, 1, NULL);
  xTaskCreate(forward_serial_task, "forward_serial", 2048, NULL, 2, NULL);
  xTaskCreate(forward_hostserial_task, "forward_hostserial", 2048, NULL, 3, NULL);
}

void loop() {
}

extern "C" {
  void tuh_cdc_mount_cb(uint8_t idx) {
    SerialHost.mount(idx);
  }

  void tuh_cdc_umount_cb(uint8_t idx) {
    SerialHost.umount(idx);
  }
}