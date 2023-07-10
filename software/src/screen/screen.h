#ifndef OSKD_SCREEN_H
#define OSKD_SCREEN_H

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include "../data/data.h"
#include "../settings/settings.h"

#include "./fonts/Font18.h"
#include "./fonts/Font28.h"
#include "./fonts/Font96.h"
#include "./fonts/Font164.h"

#define FONT_SMALL Baloo18
#define FONT_MEDIUM Inconsolata28
#define FONT_LARGE Inconsolata96
#define FONT_HUGE Inconsolata164

struct Position
{
    int x;
    int y;
};

class Screen
{
public:
    void setup();

    void update();

    void updateOption(int index, int value);
    void updateCursor(int index);

private:
    TFT_eSPI tft = TFT_eSPI(240, 320);

    const int SCREEN_HEIGHT = 320;
    const int SCREEN_WIDTH = 240;

    const int BACKLIGHT_PIN = 26;

    int batteryBars = -1;
    double batteryVoltage = -1;
    int speed = -1;
    int gear = -1;
    int temperature = -1;
    int power = -1;
    int gearState = NORMAL;
    View view = MAIN;

    int cursor = 0;

    void main();
    void resetMain();
    void resetSettings();

    void drawBatteryBars();
    void drawBatteryVoltage();
    void drawSpeed();
    void drawGear();
    void drawTemperature();
    void drawPower();

    void renderOption(String name, String value, Position position, bool selected);
};

extern Screen screen;

#endif // OSKD_SCREEN_H