#ifndef OSKD_IO_H
#define OSKD_IO_H

#include <Arduino.h>
#include <button.h>

#include "../data/data.h"
#include "../settings/settings.h"

class IO
{
public:
    void setup();
    void update();

private:
    const int BATTERY_VOLTAGE_PIN = 35;
    const int BATTERY_VOLTAGE_OFFSET_PIN = 34;
    const int BUTTON_DOWN_PIN = 14;
    const int BUTTON_POWER_PIN = 27;
    const int BUTTON_UP_PIN = 12;

    Button buttonDown = Button(this->BUTTON_DOWN_PIN);
    Button buttonPower = Button(this->BUTTON_POWER_PIN);
    Button buttonUp = Button(this->BUTTON_UP_PIN);

    int voltageOffset = 0;
    int getVoltageOffset();
    double getBatteryVoltage();
};

extern IO io;

#endif // OSKD_IO_H