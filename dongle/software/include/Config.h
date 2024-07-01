#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
#define USB_HOST_VCC_PIN 42
#define USB_HOST_CS_PIN 38
#define USB_HOST_IRQ_PIN 35

// Serial settings
#define SERIAL_BAUD_RATE 115200
#define USB_HOST_BAUD_RATE 115200

// BLE settings
#define BLE_DEVICE_NAME "OpenPendant"
#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-cba987654321"

// OTA settings
#define OTA_HOSTNAME "openpendant"
#define OTA_BUTTON_PIN 0

#endif