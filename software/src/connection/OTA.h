#ifndef OSKD_OTA_H
#define OSKD_OTA_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "./credentials.h" // ssid and password

IPAddress setupOTA()
{
    ArduinoOTA.setHostname("OSKD");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    WiFi.waitForConnectResult(2000);

    ArduinoOTA.begin();

    return WiFi.localIP();
}

#endif // OSKD_OTA_H