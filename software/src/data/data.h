#ifndef OSKD_DATA_H
#define OSKD_DATA_H

#include <Arduino.h>

enum GEAR_STATE
{
    NORMAL,
    LIMIT,
    BRAKE
};

class Data
{
private:
    static const int BUFFER_SIZE_UP = 13;

public:
    int batteryBars = 0;
    double batteryVoltage = 0;
    int speed = 0;
    int gear = 0;
    int temperature = 0;
    int power = 0;
    GEAR_STATE gearState = NORMAL;

    byte settingsBuffer[BUFFER_SIZE_UP];
};

extern Data data;

#endif // OSKD_DATA_H