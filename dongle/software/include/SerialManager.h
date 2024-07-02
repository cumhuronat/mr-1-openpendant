#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <Arduino.h>
#include "USBManager.h"

class SerialManager
{
public:
    void begin();
    static void forwardSerialTask(void *param);

private:
    static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

    static bool mr1Ready;
};

extern SerialManager serialManager;
#endif