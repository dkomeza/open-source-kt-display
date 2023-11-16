#include <Arduino.h>

#include "connection/OTA.h"

long lastTime = 0;

void setup()
{
    lastTime = millis();

    setupOTA();
}

void loop()
{
    ArduinoOTA.handle();

    // Consistent loop time (10ms)
    long currentTime = millis();
    int timeDiff = currentTime - lastTime;
    if (timeDiff < 10)
    {
        delay(10 - timeDiff);
    }
    lastTime = millis();
}