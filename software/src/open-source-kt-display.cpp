#include <Arduino.h>

#include "connection/OTA.h"
#include "controller/controller.h"
#include "data/data.h"
#include "io/io.h"
#include "screen/screen.h"
#include "settings/settings.h"

void setup()
{
    setupOTA();

    Controller controller;
    Settings settings;
}

void loop()
{
    ArduinoOTA.handle();
}