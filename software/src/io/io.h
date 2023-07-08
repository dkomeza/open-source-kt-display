#ifndef OSKD_IO_H
#define OSKD_IO_H

#include <Arduino.h>

#include "../data/data.h"

class IO
{
public:
    void setup();
    void update();

private:
    const int BATTERY_VOLTAGE_PIN = 35;

    double getBatteryVoltage();
};

extern IO io;

#endif // OSKD_IO_H