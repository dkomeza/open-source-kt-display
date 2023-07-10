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
    if (data.view != view)
    {
        view = data.view;
        switch (view)
        {
        case MAIN:
            resetMain();
            break;
        case SETTINGS:
            resetSettings();
            break;
        }
    }

    if (data.view == MAIN)
    {
        main();
    }
}

void Screen::resetMain()
{
    tft.fillScreen(TFT_BLACK);

    tft.drawRoundRect(10, 6, 80, 24, 4, TFT_GREEN);
    tft.fillRect(90, 13, 3, 10, TFT_GREEN);
    tft.fillRoundRect(92, 13, 3, 10, 2, TFT_GREEN);

    tft.drawFastHLine(0, 52, 240, TFT_WHITE);
    tft.drawFastHLine(0, 284, 240, TFT_WHITE);

    drawBatteryBars();
    drawBatteryVoltage();
    drawSpeed();
    drawGear();
    drawTemperature();
    drawPower();
}

void Screen::main()
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

    if (data.gearState != gearState || data.gear != gear)
    {
        gear = data.gear;
        gearState = data.gearState;
        drawGear();
    }

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
    tft.loadFont(FONT_MEDIUM);

    String voltage = String(batteryVoltage, 1) + "V";

    if (batteryVoltage < 9.95)
    {
        voltage = "0" + voltage;
    }

    tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(voltage, 158, 8);
    tft.unloadFont();
}

void Screen::drawSpeed()
{
    tft.loadFont(FONT_HUGE);

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

    int padding = tft.textWidth("00");
    tft.setTextPadding(padding);
    tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(speedString, SCREEN_WIDTH / 2 - padding / 2, SCREEN_HEIGHT / 2 - 24);
    tft.unloadFont();
}

void Screen::drawGear()
{
    tft.loadFont(FONT_LARGE);

    if (gear < 0)
    {
        gear = 0;
    }
    if (gear > 6)
    {
        gear = 6;
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

    int padding = tft.textWidth("0");
    tft.setTextPadding(padding);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(String(gear), SCREEN_WIDTH / 2 - padding / 2, 204);
    tft.unloadFont();
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

    int padding = tft.textWidth("-000°C");
    tft.setTextDatum(TL_DATUM);
    tft.drawString(temperatureString, x + 6, y);
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

    int padding = tft.textWidth("0000W");
    tft.setTextDatum(TL_DATUM);
    tft.setTextPadding(padding);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
    tft.drawString(powerString, x + 6, y);
    tft.unloadFont();
}

void Screen::resetSettings()
{
    tft.fillScreen(TFT_BLACK);
    tft.loadFont(FONT_MEDIUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("settings", SCREEN_WIDTH / 2, 8);
    tft.unloadFont();
    tft.drawFastHLine(0, 36, SCREEN_WIDTH, TFT_WHITE);

    settings.cursorPosition = 0;

    for (int i = 0; i < settings.menuSize; i++)
    {
        int x = i < 9 ? 20 : SCREEN_WIDTH / 2 + 50;
        int y = 50 + i % 9 * 20;

        String name = settings.getOptionName(i);
        int value = settings.getOptionValue(i);
        Position position = {x, y};
        bool selected = settings.cursorPosition == i && settings.selectedOption;

        String valueString = String(value);

        if (i == 1)
        {
            double size = settings.getWheelSize(value) / 10.0;

            valueString = String(size, 1);
        }

        renderOption(name, valueString, position, selected);
    }

    updateCursor(0);
}

void Screen::renderOption(String name, String value, Position position, bool selected)
{
    String paddingString = "000.0";
    int padding = tft.textWidth(paddingString);

    tft.loadFont(FONT_SMALL);
    tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
    tft.setTextDatum(TL_DATUM);
    int offset = tft.drawString(name, position.x, position.y) + 6;
    if (selected)
    {
        tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
    }
    tft.setTextPadding(padding);
    tft.drawString(value, position.x + offset, position.y);
    tft.setTextPadding(0);
    tft.unloadFont();
}

void Screen::updateOption(int index, int value)
{
    int x = index < 9 ? 20 : SCREEN_WIDTH / 2 + 50;
    int y = 50 + index % 9 * 20;

    String name = settings.getOptionName(index);
    Position position = {x, y};
    bool selected = settings.cursorPosition == index && settings.selectedOption;

    String valueString = String(value);

    if (index == 1)
    {
        double size = settings.getWheelSize(value) / 10.0;

        valueString = String(size, 1);
    }

    renderOption(name, valueString, position, selected);
}

void Screen::updateCursor(int index)
{
    int oldX = cursor < 9 ? 10 : SCREEN_WIDTH / 2 + 40;
    int oldY = 56 + cursor % 9 * 20;
    tft.fillTriangle(oldX - 4, oldY - 4, oldX - 4, oldY + 4, oldX + 4, oldY, TFT_BLACK);

    int x = index < 9 ? 10 : SCREEN_WIDTH / 2 + 40;
    int y = 56 + index % 9 * 20;
    tft.fillTriangle(x - 4, y - 4, x - 4, y + 4, x + 4, y, TFT_YELLOW);

    cursor = index;
}
