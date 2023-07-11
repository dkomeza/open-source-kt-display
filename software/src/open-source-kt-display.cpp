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

const int POWER_PIN = 19;
const int POWER_BUTTON_PIN = 27;

void handleWakeUp();

void setup()
{
    gpio_hold_dis(GPIO_NUM_19);
    gpio_hold_dis(GPIO_NUM_26);

    pinMode(POWER_PIN, OUTPUT);
    pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)POWER_BUTTON_PIN, 0);

    handleWakeUp();

    controller.setup();
    screen.setup();
    io.setup();
    settings.setup();

    screen.setBrightness(1);

    setupOTA();

    delay(400);
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

    lastUpdate = millis();
}

void handleWakeUp()
{
    if (digitalRead(POWER_BUTTON_PIN) != LOW)
    {
        esp_deep_sleep_start();
    }
    delay(1000);
    if (digitalRead(POWER_BUTTON_PIN) != LOW)
    {
        esp_deep_sleep_start();
    }
    digitalWrite(POWER_PIN, HIGH);
}