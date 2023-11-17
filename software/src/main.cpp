#include <Arduino.h>

#include "connection/OTA.h"
#include "button/buttons.h"

long lastTime = 0;

void setup()
{
    setupOTA();
    Buttons::setup();

    lastTime = millis();
}

void loop()
{
    ArduinoOTA.handle();
    Buttons::update();

    // Consistent loop time (10ms)
    long currentTime = millis();
    int timeDiff = currentTime - lastTime;
    if (timeDiff < 10)
    {
        delay(10 - timeDiff);
    }
    lastTime = millis();
}