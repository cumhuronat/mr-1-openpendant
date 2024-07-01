#include "USBManager.h"

Adafruit_USBH_CDC USBManager::SerialHost;

USBManager::USBManager() : USBHost(&SPI, USB_HOST_CS_PIN, USB_HOST_IRQ_PIN) {}

void USBManager::begin()
{
    pinMode(USB_HOST_VCC_PIN, OUTPUT);
    digitalWrite(USB_HOST_VCC_PIN, HIGH);
    USBHost.begin(1);
    SerialHost.begin(USB_HOST_BAUD_RATE);
    xTaskCreatePinnedToCore(usbHostTask, "usbh", 2048, NULL, 3, NULL, 1);
}

void USBManager::usbHostTask(void *param)
{
    while (1)
    {
        usbManager.USBHost.task();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

extern "C"
{
    void tuh_cdc_mount_cb(uint8_t idx)
    {
        USBManager::getSerialHost().mount(idx);
    }

    void tuh_cdc_umount_cb(uint8_t idx)
    {
        USBManager::getSerialHost().umount(idx);
    }
}