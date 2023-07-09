#include "./screen.h"

void Screen::setup()
{
    tft.init();
    tft.setRotation(4);

    tft.fillScreen(TFT_BLACK);

    tft.drawRoundRect(10, 6, 80, 24, 4, TFT_GREEN);
    tft.fillRect(90, 13, 3, 10, TFT_GREEN);
    tft.fillRoundRect(92, 13, 3, 10, 2, TFT_GREEN);

    tft.drawFastHLine(0, 52, 240, TFT_WHITE);
    tft.drawFastHLine(0, 284, 240, TFT_WHITE);

    delay(50);
    pinMode(BACKLIGHT_PIN, OUTPUT);
    dacWrite(BACKLIGHT_PIN, 255);
}

void Screen::update()
{
    if (data.batteryBars != batteryBars)
    {
        batteryBars = data.batteryBars;
        drawBatteryBars();
    }

    tft.loadFont(FONT_MEDIUM);
    if (data.batteryVoltage != batteryVoltage)
    {
        batteryVoltage = data.batteryVoltage;
        drawBatteryVoltage();
    }
    tft.unloadFont();

    tft.loadFont(FONT_HUGE);
    if (data.speed != speed)
    {
        speed = data.speed;
        drawSpeed();
    }
    tft.unloadFont();

    tft.loadFont(FONT_LARGE);
    if (data.gear != gear || data.gearState != gearState)
    {
        gear = data.gear;
        drawGear();
    }
    tft.unloadFont();

    if (data.temperature != temperature)
    {
        temperature = data.temperature;
        drawTemperature();
    }

    if (data.power != power)
    {
        power = data.power;
        drawPower();
    }
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

    tft.fillRoundRect(11, 7, 78, 22, 4, TFT_BLACK); // Clear battery bars

    for (int i = 0; i < bars; i++)
    {
        tft.fillSmoothRoundRect(13 + i * 19, 9, 17, 18, 2, TFT_GREEN);
    }
}

void Screen::drawBatteryVoltage()
{
    String voltage = String(batteryVoltage, 1) + "V";

    if (batteryVoltage < 9.95)
    {
        voltage = "0" + voltage;
    }

    tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(voltage, 158, 8);
}

void Screen::drawSpeed()
{
    if (speed >= 100)
    {
        speed = 99;
    }

    if (speed < 0)
    {
        speed = 0;
    }

    String speedString = String(speed);

    if (speed < 10)
    {
        speedString = "0" + speedString;
    }

    tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(speedString, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 24);
}

void Screen::drawGear()
{
    if (gear < 0)
    {
        gear = 0;
    }
    if (gear > 5)
    {
        gear = 5;
    }

    if (gearState & BRAKE)
    {
        tft.setTextColor(TFT_RED, TFT_BLACK, true);
    }
    else if (gearState & LIMIT)
    {
        tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
    }
    else
    {
        tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
    }

    tft.setTextDatum(TC_DATUM);
    tft.drawString(String(data.gear), SCREEN_WIDTH / 2, 204);
}

void Screen::drawTemperature()
{
    int x = 4;
    int y = 296;

    tft.loadFont(FONT_MEDIUM);
    String labelString = String("T:");
    String temperatureString = String(temperature) + "°C";

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
    int offset = tft.drawString(labelString, x, y);
    x += offset;

    tft.setTextColor(TFT_BLACK, TFT_BLACK, true);

    if (temperature > -100 && temperature <= -10)
    {
        x += tft.drawString("0", x, y);
    }
    else if (temperature > -10 && temperature < 0)
    {
        x += tft.drawString("00", x, y);
    }
    else if (temperature >= 0 && temperature < 10)
    {
        x += tft.drawString("000", x, y);
    }
    else if (temperature >= 10 && temperature < 100)
    {
        x += tft.drawString("00", x, y);
    }
    else if (temperature >= 100 && temperature < 1000)
    {
        x += tft.drawString("0", x, y);
    }

    if (temperature < 0)
    {
        tft.setTextColor(TFT_BLUE, TFT_BLACK, true);
    }
    else if (temperature < 25)
    {
        tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
    }
    else if (temperature < 70)
    {
        tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
    }
    else if (temperature < 90)
    {
        tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
    }
    else
    {
        tft.setTextColor(TFT_RED, TFT_BLACK, true);
    }
    tft.drawString(temperatureString, x, y);
    tft.unloadFont();
}

void Screen::drawPower()
{
    int x = 134;
    int y = 296;

    tft.loadFont(FONT_MEDIUM);
    String labelString = String("P:");
    String powerString = String(power) + "W";

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    int offset = tft.drawString(labelString, x, y);
    x += offset;

    tft.setTextColor(TFT_BLACK, TFT_BLACK, true);
    if (power < 10)
    {
        x += tft.drawString("000", x, y);
    }
    else if (power < 100)
    {
        x += tft.drawString("00", x, y);
    }
    else if (power < 1000)
    {
        x += tft.drawString("0", x, y);
    }

    tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
    tft.drawString(powerString, x, y);
    tft.unloadFont();
}
