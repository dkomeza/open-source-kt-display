#include "./settings.h"

void Settings::setup()
{
    EEPROM.begin(EEPROM_SIZE);

    loadSettings();
    loadGearFromEEPROM();
    loadLegalModeFromEEPROM();
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
        (((speedLimit - 10) & 31) << 3) | (wheelSizeTable[*WHEEL_SIZE][1] >> 2);
    settingsBuffer[3] = *P1;
    settingsBuffer[4] =
        ((wheelSizeTable[*WHEEL_SIZE][1] & 3) << 6) |
        ((speedLimit - 10) & 32) |
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
    previousGear = currentGear;
    if (gear > MAX_GEAR)
    {
        gear = MAX_GEAR;
    }
    else if (gear < MIN_GEAR)
    {
        gear = MIN_GEAR;
    }

    if (legalMode && gear > 2)
    {
        gear = 2;
    }

    currentGear = gear;
    data.gear = gear;

    this->calculatePacket();
    this->saveGearToEEPROM();
}

void Settings::toggleLegalMode()
{
    legalMode = !legalMode;

    if (legalMode)
    {
        data.gearState |= LIMIT;
        speedLimit = 25;
        if (currentGear > 2)
        {
            currentGear = 2;
            data.gear = currentGear;
        }
    }
    else
    {
        data.gearState ^= LIMIT;
        speedLimit = *SPEED_LIMIT;
    }

    this->calculatePacket();
    this->saveLegalModeToEEPROM();
    this->saveGearToEEPROM();
}

void Settings::saveGearToEEPROM()
{
    EEPROM.write(GEAR_EEPROM_ADDRESS, currentGear);
    EEPROM.commit();
}

void Settings::loadGearFromEEPROM()
{
    currentGear = EEPROM.read(GEAR_EEPROM_ADDRESS);
    data.gear = currentGear;
    this->calculatePacket();
}

void Settings::saveLegalModeToEEPROM()
{
    EEPROM.write(LEGAL_MODE_EEPROM_ADDRESS, legalMode);
    EEPROM.commit();
}

void Settings::loadLegalModeFromEEPROM()
{
    legalMode = EEPROM.read(LEGAL_MODE_EEPROM_ADDRESS);
    speedLimit = legalMode ? 25 : *SPEED_LIMIT;

    if (legalMode)
    {
        data.gearState |= LIMIT;
        if (currentGear > 2)
        {
            currentGear = 2;
            data.gear = 2;
        }
    }
    else
    {
        data.gearState = NORMAL;
    }

    this->calculatePacket();
}

void Settings::startWalkMode()
{
    previousGear = currentGear;
    currentGear = 6;
    data.gear = currentGear;
    this->calculatePacket();
}

void Settings::stopWalkMode()
{
    currentGear = previousGear;
    data.gear = previousGear;

    this->calculatePacket();
}

void Settings::increaseOption()
{
    if (selectedOption)
    {
        values[cursorPosition] += 1;
        if (values[cursorPosition] > maxValues[cursorPosition])
        {
            values[cursorPosition] = minValues[cursorPosition];
        }
        screen.updateOption(cursorPosition, values[cursorPosition]);
    }
    else
    {
        cursorPosition += 1;
        if (cursorPosition > MENU_SIZE - 1)
        {
            cursorPosition = 0;
        }
        screen.updateCursor(cursorPosition);
    }
}

void Settings::decreaseOption()
{
    if (selectedOption)
    {
        values[cursorPosition] -= 1;
        if (values[cursorPosition] < minValues[cursorPosition])
        {
            values[cursorPosition] = maxValues[cursorPosition];
        }
        screen.updateOption(cursorPosition, values[cursorPosition]);
    }
    else
    {
        cursorPosition -= 1;
        if (cursorPosition < 0)
        {
            cursorPosition = MENU_SIZE - 1;
        }
        screen.updateCursor(cursorPosition);
    }
}

void Settings::selectOption()
{
    selectedOption = !selectedOption;
}
