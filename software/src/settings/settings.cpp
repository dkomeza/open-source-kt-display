#include "./settings.h"

void Settings::setup()
{
    EEPROM.begin(EEPROM_SIZE);

    loadSettings();
}

void Settings::loadSettings()
{
    bool settingsValid = checkInitialSettings();

    if (settingsValid)
    {
        for (int i = 0; i < MENU_SIZE; i++)
        {
            values[i] = EEPROM.read(i);
        }
    }
    else
    {
        for (int i = 0; i < MENU_SIZE; i++)
        {
            values[i] = defaultValues[i];
        }
    }
}

bool Settings::checkInitialSettings()
{
    for (int i = 0; i < MENU_SIZE; i++)
    {
        if (EEPROM.read(i) > maxValues[i] || EEPROM.read(i) < minValues[i])
        {
            return false;
        }
    }
    return true;
}

void Settings::saveSettings()
{
    for (int i = 0; i < MENU_SIZE; i++)
    {
        EEPROM.write(i, values[i]);
    }
    EEPROM.commit();
}

void Settings::calculatePacket()
{
    settingsBuffer[0] = *P5;
    settingsBuffer[1] = currentGear;
    settingsBuffer[2] =
        (((*SPEED_LIMIT - 10) & 31) << 3) | (wheelSizeTable[*WHEEL_SIZE][1] >> 2);
    settingsBuffer[3] = *P1;
    settingsBuffer[4] =
        ((wheelSizeTable[*WHEEL_SIZE][1] & 3) << 6) |
        ((*SPEED_LIMIT - 10) & 32) |
        *P4 << 4 | *P3 << 3 | *P2;
    settingsBuffer[5] = 0;
    settingsBuffer[6] = (*C1 << 3) | *C2;
    settingsBuffer[7] = (*C14 << 5) | *C5 | 128;
    settingsBuffer[8] = (*C4 << 5) | *C12;
    settingsBuffer[9] = 20;
    settingsBuffer[10] = *C13 << 2 | 1;
    settingsBuffer[11] = 50;
    settingsBuffer[12] = 14;

    settingsBuffer[5] = calculateChecksum(settingsBuffer);

    memcpy(data.settingsBuffer, settingsBuffer, sizeof(settingsBuffer));
}

byte Settings::calculateChecksum(byte *buffer)
{
    byte checksum = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (i == 5)
        {
            continue;
        }
        checksum ^= buffer[i];
    }

    checksum ^= 3;
    return checksum;
}

void Settings::setGear(int gear)
{
    if (gear > MAX_GEAR)
    {
        gear = MAX_GEAR;
    }
    else if (gear < MIN_GEAR)
    {
        gear = MIN_GEAR;
    }

    currentGear = gear;

    this->calculatePacket();
}

void Settings::setSpeedLimit(int speedLimit)
{
    if (speedLimit > MAX_SPEED_LIMIT)
    {
        speedLimit = MAX_SPEED_LIMIT;
    }
    else if (speedLimit < MIN_SPEED_LIMIT)
    {
        speedLimit = MIN_SPEED_LIMIT;
    }

    *SPEED_LIMIT = speedLimit;

    this->calculatePacket();
}
