#include "./io.h"

void IO::setup()
{
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    this->voltageOffset = this->getVoltageOffset();
}

void IO::update()
{
    data.batteryVoltage = this->getBatteryVoltage();
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