#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>

#include "credentials.h"

IPAddress setupOTA(const char* nameprefix) {
  ArduinoOTA.setHostname(nameprefix);

  // Configure and start the WiFi station
  WiFi.mode(WIFI_STA);
  WiFi.begin(mySSID, myPASSWORD);

  // Wait for connection
  WiFi.waitForConnectResult(5000);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else  // U_SPIFFS
      type = "filesystem";
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("\nAuth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("\nBegin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("\nConnect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("\nReceive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("\nEnd Failed");
  });

  ArduinoOTA.begin();

  Serial.println("OTA Initialized");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  return WiFi.localIP();
}