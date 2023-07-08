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

void setup()
{
    pinMode(26, OUTPUT);
    dacWrite(26, 200);

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

    ArduinoOTA.handle();

    if (millis() - lastUpdate < 50)
    {
        delay(50 - (millis() - lastUpdate));
    }
}