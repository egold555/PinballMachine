#include "Net.h"

#if __has_include("secrets.h")
#include "secrets.h"
#define HAVE_SECRETS 1
#endif

#ifdef HAVE_SECRETS

#include <WiFi.h>
#include <ArduinoOTA.h>

namespace
{
    const char *HOSTNAME = "pinball";
    Machine *machine = nullptr;

    void netTask(void *)
    {
        WiFi.setHostname(HOSTNAME);
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        bool otaStarted = false;
        for (;;)
        {
            if (WiFi.status() == WL_CONNECTED && !otaStarted)
            {
                Serial.printf("[net] connected, %s.local (%s)\n", HOSTNAME, WiFi.localIP().toString().c_str());
                ArduinoOTA.setHostname(HOSTNAME);
                ArduinoOTA.onStart([]() {
                    Serial.println("[net] OTA update starting");
                    machine->lockOut("UPDATING");
                });
                ArduinoOTA.onError([](ota_error_t error) {
                    Serial.printf("[net] OTA failed (%u), restarting\n", error);
                    ESP.restart();
                });
                ArduinoOTA.begin();
                otaStarted = true;
            }
            if (otaStarted)
                ArduinoOTA.handle();
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void net::begin(Machine &m)
{
    machine = &m;
    xTaskCreate(netTask, "net", 6144, nullptr, 1, nullptr);
}

#else

void net::begin(Machine &)
{
    Serial.println("[net] no src/secrets.h, WiFi and OTA are off");
}

#endif
