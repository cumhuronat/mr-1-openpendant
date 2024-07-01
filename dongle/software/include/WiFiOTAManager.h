#ifndef WifiOTA_MANAGER_H
#define WifiOTA_MANAGER_H

#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include "Config.h"

class WiFiOTAManager
{
public:
    void begin();

private:
    void setupOTA();
    void checkOTAButton();
    static void otaHandleTask(void *param);
};

extern WiFiOTAManager wifiOtaManager;

#endif