#include "Adafruit_TinyUSB.h"
#include "SPI.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
// #include "ESPTelnet.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-cba987654321"

Adafruit_USBH_Host USBHost(&SPI, 38, 35);
Adafruit_USBH_CDC SerialHost;
BLECharacteristic *pCharacteristic;
// ESPTelnet telnet;

bool mr1Ready = false;

void processMessage(const String& message) {
  String msg = "[MSG:" + message + "]";
  Serial.println(msg);
  Serial.flush();
}

void forward_serial_task(void *param) {
  (void)param;
  uint8_t buf[64];
  static String serialHostBuffer;

  while (1) {
    size_t count = Serial.available();
    if (count > 0) {
      if (count > 64) count = 64;
      size_t bytesRead = Serial.read(buf, count);
      if (bytesRead > 0 && mr1Ready) {
        SerialHost.write(buf, bytesRead);
        SerialHost.flush();
      }
    }

    count = SerialHost.available();
    if (count > 0) {
      if (count > 64) count = 64;
      size_t bytesRead = SerialHost.read(buf, count);
      if (bytesRead > 0) {
        Serial.write(buf, bytesRead);
        Serial.flush();
        for (size_t i = 0; i < bytesRead; ++i) {
          char c = buf[i];
          if (c == '\r' || c == '\0') {
            // Ignore carriage return and null character
            continue;
          } else if (c == '\n') {
            // Process the complete message
            processMessage(serialHostBuffer);
            serialHostBuffer = ""; // Clear the buffer
          } else {
            serialHostBuffer += c; // Add character to buffer
          }
        }
      }
      mr1Ready = true;
    }

    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}


void ota_handle_task( void * param ) {
  (void)param;
  while (1) {
    ArduinoOTA.handle();
    vTaskDelay(3500 / portTICK_PERIOD_MS);
  }
}
/*
void telnet_task( void * param ) {
  while (1) {
    telnet.loop();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}*/

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

void setOTA() {
  ArduinoOTA.setHostname("openpendant");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setAutoReconnect(true);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.begin();
}
/*
int telnetVprintf(const char *fmt, va_list args) {
  // direct to telnet
  char buf[256];
  vsnprintf(buf, sizeof(buf), fmt, args);
  telnet.print(buf);
  return strlen(buf);
}*/

void setup() {
  pinMode(42, OUTPUT);
  digitalWrite(42, HIGH);
  USBHost.begin(1);
  SerialHost.begin(115200);
  Serial.enableReboot(false);
  Serial.begin(115200);
  Serial.onEvent(usbEventCallback);
  setBle();
  // setOTA();
  // telnet.begin(23);
  // esp_log_set_vprintf(telnetVprintf);

  xTaskCreatePinnedToCore(usbhost_rtos_task, "usbh", 2048, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(forward_serial_task, "forward_serial", 2048, NULL, 2, NULL, 1);
  // xTaskCreatePinnedToCore(ota_handle_task, "ota_handle", 10000, NULL, tskIDLE_PRIORITY, NULL, 1);
  // xTaskCreatePinnedToCore(telnet_task, "telnet", 10000, NULL, tskIDLE_PRIORITY, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}

extern "C" {
  void tuh_cdc_mount_cb(uint8_t idx) {
    SerialHost.mount(idx);
  }

  void tuh_cdc_umount_cb(uint8_t idx) {
    SerialHost.umount(idx);
  }
}