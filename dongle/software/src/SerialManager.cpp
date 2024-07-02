#include "SerialManager.h"
#include "GRBLParser.h"

bool SerialManager::mr1Ready = false;

void SerialManager::begin()
{
    xTaskCreatePinnedToCore(forwardSerialTask, "forward_serial", 4096, NULL, 2, NULL, 1);
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.enableReboot(false);
    Serial.onEvent(usbEventCallback);
}

void SerialManager::forwardSerialTask(void *param)
{
    uint8_t buf[64];
    static String serialHostBuffer;

    while (1)
    {
        size_t count = Serial.available();
        if (count > 0)
        {
            if (count > 64)
                count = 64;
            size_t bytesRead = Serial.read(buf, count);
            if (bytesRead > 0 && mr1Ready)
            {
                USBManager::getSerialHost().write(buf, bytesRead);
                USBManager::getSerialHost().flush();
            }
        }

        count = USBManager::getSerialHost().available();
        if (count > 0)
        {
            if (count > 64)
                count = 64;
            size_t bytesRead = USBManager::getSerialHost().read(buf, count);
            if (bytesRead > 0)
            {
                Serial.write(buf, bytesRead);
                Serial.flush();
                for (size_t i = 0; i < bytesRead; ++i)
                {
                    char c = buf[i];
                    if (c == '\r' || c == '\0')
                    {
                        continue;
                    }
                    else if (c == '\n')
                    {
                        GRBLParser::processMessage(serialHostBuffer);
                        serialHostBuffer = "";
                    }
                    else
                    {
                        serialHostBuffer += c;
                    }
                }
            }
            mr1Ready = true;
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}




void SerialManager::usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == ARDUINO_USB_CDC_EVENTS)
    {
        arduino_usb_cdc_event_data_t *data = (arduino_usb_cdc_event_data_t *)event_data;
        if (event_id == ARDUINO_USB_CDC_LINE_STATE_EVENT && data->line_state.dtr == 1 && data->line_state.rts == 1)
        {
            USBManager::getSerialHost().write(0x18);
            USBManager::getSerialHost().flush();
            mr1Ready = false;
        }
    }
}