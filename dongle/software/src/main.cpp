#include <Arduino.h>
#include "USBManager.h"
#include "BLEManager.h"
#include "WiFiOTAManager.h"
#include "SerialManager.h"
#include "Config.h"

USBManager usbManager;
BLEManager bleManager;
WiFiOTAManager wifiOtaManager;
SerialManager serialManager;

void setup()
{
    usbManager.begin();
    bleManager.begin();
    wifiOtaManager.begin();
    serialManager.begin();
}

void loop()
{
    vTaskDelete(NULL);
}