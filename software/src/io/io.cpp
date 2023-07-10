#include "./io.h"

void handleDownClick();
void handleUpClick();
void handlePowerDoubleClick();

void IO::setup()
{
    pinMode(this->BATTERY_VOLTAGE_PIN, INPUT);
    this->voltageOffset = this->getVoltageOffset();

    this->buttonDown.onClick(handleDownClick);
    this->buttonDown.onLongPressStart([]()
                                      { settings.startWalkMode(); });
    this->buttonDown.onLongPressStop([]()
                                     { settings.stopWalkMode(); });

    this->buttonUp.onClick(handleUpClick);
    this->buttonUp.onLongPressStart([]()
                                    { settings.toggleLegalMode(); });

    this->buttonPower.onDoubleClick(handlePowerDoubleClick);
    this->buttonPower.onClick(handlePowerClick);
}

void IO::update()
{
    data.batteryVoltage = this->getBatteryVoltage();

    this->buttonDown.update();
    this->buttonPower.update();
    this->buttonUp.update();
}

int IO::getVoltageOffset()
{
    int reference = map(1815, 0, 3300, 0, 4095);

    int sum = 0;

    for (int i = 0; i < 10; i++)
    {
        sum += analogRead(this->BATTERY_VOLTAGE_OFFSET_PIN);
    }

    int average = sum / 10;

    average = reference - average;

    return average;
}

double IO::getBatteryVoltage()
{
    int sum = 0;

    for (int i = 0; i < 10; i++)
    {
        sum += analogRead(this->BATTERY_VOLTAGE_PIN);
    }

    int average = sum / 10;
    average += this->voltageOffset;

    double voltage = map(average, 0, 4095, 0, 3300);

    double batteryVoltage = voltage * (1000 + 56) / 56;

    return batteryVoltage / 1000;
}

void handleDownClick()
{
    switch (data.view)
    {
    case MAIN:
        if (data.gear > 0)
        {
            settings.setGear(data.gear - 1);
        }
    case SETTINGS:
        settings.decreaseOption();
        break;
    }
}

void handleUpClick()
{
    switch (data.view)
    {
    case MAIN:
        if (data.gear < 5)
        {
            settings.setGear(data.gear + 1);
        }
        break;
    case SETTINGS:
        settings.increaseOption();
        break;
    }
}

void handlePowerDoubleClick()
{
    data.view = data.view == MAIN ? SETTINGS : MAIN;
}

void handlePowerClick()
{
    switch (data.view)
    {
    case SETTINGS:
        settings.selectOption();
        break;
    }
}
