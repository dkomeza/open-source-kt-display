#include "./screen.h"

void Screen::setup()
{
    tft.init();
    tft.setRotation(1);

    tft.fillScreen(TFT_BLACK);

    tft.drawSmoothRoundRect(10, 16, 4, 2, 80, 24, TFT_GREEN);
    tft.fillRect(90, 23, 3, 10, TFT_GREEN);
    tft.fillSmoothRoundRect(92, 23, 3, 10, 2, TFT_GREEN);

    tft.drawFastHLine(10, 54, 220, TFT_WHITE);
    tft.drawFastHLine(10, 248, 220, TFT_WHITE);
}

void Screen::update()
{
    if (data.batteryBars != batteryBars)
    {
        batteryBars = data.batteryBars;
        drawBatteryBars();
    }

    if (data.batteryVoltage != batteryVoltage)
    {
        batteryVoltage = data.batteryVoltage;
        drawBatteryVoltage();
    }

    if (data.speed != speed)
    {
        speed = data.speed;
        drawSpeed();
    }

    if (data.gear != gear || data.gearState != gearState)
    {
        gear = data.gear;
        drawGear();
    }

    // if (data.temperature != temperature)
    // {
    //     temperature = data.temperature;
    //     drawTemperature();
    // }

    // if (data.power != power)
    // {
    //     power = data.power;
    //     drawPower();
    // }
}

void Screen::drawBatteryBars()
{
    int bars = (batteryBars + 3) / 4;

    if (bars < 0)
    {
        bars = 0;
    }
    if (bars > 4)
    {
        bars = 4;
    }

    tft.fillSmoothRoundRect(11, 17, 78, 22, 2, TFT_BLACK); // Clear battery bars

    for (int i = 0; i < bars; i++)
    {
        tft.fillSmoothRoundRect(13 + i * 19, 19, 17, 18, 2, TFT_GREEN);
    }
}

void Screen::drawBatteryVoltage()
{
    String voltage = String(batteryVoltage, 1) + "V";

    tft.setTextSize(1);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(voltage, 164, 16);
}

void Screen::drawSpeed()
{
    String speedString = String(speed) + "km/h";

    if (speed < 10)
    {
        speedString = "0" + speedString;
    }

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(speedString, 10, 60);

    if (speed >= 100)
    {
        speed = 99;
    }

    if (speed < 0)
    {
        speed = 0;
    }

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextFont(8);
    tft.setTextSize(1);

    tft.drawString(speedString, 30, 86);
}

void Screen::drawGear()
{
    tft.setTextFont(7);
    tft.setTextSize(1);

    switch (data.gearState)
    {
    case NORMAL:
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        break;
    case LIMIT:
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        break;
    case BRAKE:
        tft.setTextColor(TFT_RED, TFT_BLACK);
        break;
    }

    tft.drawString(String(data.gear), 108, 180);
}
