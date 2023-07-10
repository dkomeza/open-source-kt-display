#ifndef OSKD_DATA_H
#define OSKD_DATA_H

#include <Arduino.h>

enum GEAR_STATE
{
    NORMAL = 1,
    LIMIT = 2,
    BRAKE = 4,
};

enum View
{
    MAIN,
    SETTINGS,
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
    int temperature = -120;
    int power = 1300;
    int gearState = NORMAL;
    View view = MAIN;

    byte settingsBuffer[BUFFER_SIZE_UP];
};

extern Data data;

#endif // OSKD_DATA_H