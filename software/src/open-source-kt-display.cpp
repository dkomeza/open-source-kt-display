#include <Arduino.h>

#include "connection/OTA.h"
#include "controller/controller.h"
#include "data/data.h"
#include "io/io.h"
#include "screen/screen.h"
#include "settings/settings.h"

Controller controller;
Data data;
IO io;
Screen screen;
Settings settings;

long lastUpdate = 0;

bool rising = true;

void setup()
{
    setupOTA();

    controller.setup();
    screen.setup();
    io.setup();
    settings.setup();

    lastUpdate = millis();
}

void loop()
{
    controller.update();
    screen.update();
    io.update();

    if (rising)
    {
        if (data.power >= 1500)
        {
            rising = false;
        }
        data.power += 13;
    }
    else
    {
        if (data.power <= 13)
        {
            rising = true;
        }
        data.power -= 13;
    }

    ArduinoOTA.handle();

    if (millis() - lastUpdate < 50)
    {
        delay(50 - (millis() - lastUpdate));
    }

    lastUpdate = millis();
}