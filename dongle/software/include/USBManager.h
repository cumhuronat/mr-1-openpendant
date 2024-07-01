#ifndef USB_MANAGER_H
#define USB_MANAGER_H

#include <Adafruit_TinyUSB.h>
#include "Config.h"

class USBManager
{
public:
    USBManager();
    void begin();
    static void usbHostTask(void *param);
    static Adafruit_USBH_CDC &getSerialHost() { return SerialHost; }

private:
    Adafruit_USBH_Host USBHost;
    static Adafruit_USBH_CDC SerialHost;
};

extern USBManager usbManager;
#endif