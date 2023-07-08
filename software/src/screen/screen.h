#ifndef OSKD_SCREEN_H
#define OSKD_SCREEN_H

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include "../data/data.h"

class Screen
{
public:
    void setup();

    void update();

private:
    TFT_eSPI tft = TFT_eSPI(SCREEN_WIDTH, SCREEN_HEIGHT);

    const int SCREEN_HEIGHT = 320;
    const int SCREEN_WIDTH = 240;

    int batteryBars = 0;
    double batteryVoltage = 0;
    int speed = 0;
    int gear = 0;
    int temperature = 0;
    int power = 0;
    GEAR_STATE gearState = NORMAL;

    void drawBatteryBars();
    void drawBatteryVoltage();
    void drawSpeed();
    void drawGear();
    // void drawTemperature();
    // void drawPower();
};

extern Screen screen;

#endif // OSKD_SCREEN_H