#ifndef OSKD_SETTINGS_H
#define OSKD_SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>

class Settings
{
private:
    static const int MENU_SIZE = 15;
    static const int BUFFER_SIZE = 13;
    static const int EEPROM_SIZE = 512;

    static const int MAX_GEAR = 5;
    static const int MIN_GEAR = 0;
    static const int MAX_SPEED_LIMIT = 72;
    static const int MIN_SPEED_LIMIT = 10;

    const String names[MENU_SIZE] = {"Speed limit", "Wheel size", "P1", "P2", "P3", "P4", "P5", "C1", "C2", "C4", "C5", "C11", "C12", "C13", "C14"};
    int values[MENU_SIZE];
    const int minValues[MENU_SIZE] = {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    const int maxValues[MENU_SIZE] = {72, 14, 255, 6, 1, 1, 30, 7, 7, 4, 10, 3, 7, 5, 3};
    const int defaultValues[MENU_SIZE] = {72, 12, 86, 1, 1, 0, 13, 5, 0, 0, 10, 0, 4, 0, 1};
    const int wheelSizeTable[15][2] = {{50, 22}, {60, 18}, {80, 10}, {100, 14}, {120, 2}, {140, 6}, {160, 0}, {180, 4}, {200, 8}, {230, 12}, {240, 16}, {260, 20}, {275, 24}, {280, 28}, {290, 30}}; // {wheel size * 10, byte value}

    int *SPEED_LIMIT = &values[0];
    int *WHEEL_SIZE = &values[1];
    int *P1 = &values[2];
    int *P2 = &values[3];
    int *P3 = &values[4];
    int *P4 = &values[5];
    int *P5 = &values[6];
    int *C1 = &values[7];
    int *C2 = &values[8];
    int *C4 = &values[9];
    int *C5 = &values[10];
    int *C11 = &values[11];
    int *C12 = &values[12];
    int *C13 = &values[13];
    int *C14 = &values[14];

    int currentGear = 0;

    bool checkInitialSettings();
    void loadSettings();
    void saveSettings();

    void calculatePacket();
    byte calculateChecksum(byte *buffer);

public:
    Settings();

    byte settingsBuffer[BUFFER_SIZE];

    void setGear(int gear);
    void setSpeedLimit(int speedLimit);
};

extern Settings settings;

#endif // OSKD_SETTINGS_H