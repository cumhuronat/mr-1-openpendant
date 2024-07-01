#include "WiFiOTAManager.h"

bool otaEnabled = false;

void WiFiOTAManager::begin()
{
#if WIFIOTA
    setupOTA();
#else
    pinMode(OTA_BUTTON_PIN, INPUT_PULLUP);
#endif
xTaskCreatePinnedToCore(WiFiOTAManager::otaHandleTask, "ota_handle", 10000, NULL, tskIDLE_PRIORITY, NULL, 1);
}


void WiFiOTAManager::checkOTAButton()
{
    if (digitalRead(OTA_BUTTON_PIN) == LOW)
    {
        delay(50);
        if (digitalRead(OTA_BUTTON_PIN) == LOW && !otaEnabled)
        {
            setupOTA();
            otaEnabled = true;
        }
    }
}

void WiFiOTAManager::otaHandleTask(void *param)
{
    while (1)
    {
        if (otaEnabled)
        {
            ArduinoOTA.handle();
            vTaskDelay(3500 / portTICK_PERIOD_MS);
        }
        else
        {
            wifiOtaManager.checkOTAButton();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

void WiFiOTAManager::setupOTA()
{
    otaEnabled = true;
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.setAutoReconnect(true);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    ArduinoOTA.begin();
}