#ifndef OSKD_SCREEN_H
#define OSKD_SCREEN_H

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include "../data/data.h"

#include "./fonts/Font28.h"
#include "./fonts/Font96.h"
#include "./fonts/Font164.h"

#define FONT_MEDIUM Inconsolata28
#define FONT_LARGE Inconsolata96
#define FONT_HUGE Inconsolata164

class Screen
{
public:
    void setup();

    void update();

private:
    TFT_eSPI tft = TFT_eSPI(240, 320);

    const int SCREEN_HEIGHT = 320;
    const int SCREEN_WIDTH = 240;

    const int BACKLIGHT_PIN = 26;

    int batteryBars = -1;
    double batteryVoltage = -1;
    int speed = -1;
    int gear = -1;
    int temperature = -30;
    int power = -1;
    GEAR_STATE gearState = NORMAL;

    void drawBatteryBars();
    void drawBatteryVoltage();
    void drawSpeed();
    void drawGear();
    void drawTemperature();
    void drawPower();
};

extern Screen screen;

#endif // OSKD_SCREEN_H