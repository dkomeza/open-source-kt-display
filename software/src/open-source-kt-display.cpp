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

void setup()
{
    setupOTA();

    controller.setup();
    screen.setup();
    io.setup();
    settings.setup();
}

void loop()
{
    controller.update();
    screen.update();
    io.update();

    ArduinoOTA.handle();
}