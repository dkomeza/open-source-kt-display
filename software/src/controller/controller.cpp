#include "./controller.h"

void Controller::setup()
{
    controllerSerial.begin(controllerBaudRate, SERIAL_8N1, controllerRXPin, controllerTXPin);
}

void Controller::update()
{
    sendData();
    receiveData();
}

void Controller::sendData()
{
    controllerSerial.write(data.settingsBuffer, BUFFER_UP_SIZE);
}

void Controller::receiveData()
{
    byte buffer[BUFFER_SIZE];
    if (controllerSerial.available() >= BUFFER_SIZE)
    {
        controllerSerial.readBytes(buffer, BUFFER_SIZE);
    }

    parseData(buffer);
}

void Controller::parseData(byte buffer[])
{
    bool isValid = isDataValid(buffer, 0);

    if (isValid)
    {
        return;
    }

    if (buffer[3] + buffer[4] <= 0)
    {
        data.speed = 0;
    }
    else
    {
        data.speed = round(60000 / (buffer[3] << 8 | buffer[4]) * RPM_CONSTANT * 0.66);
    }

    data.batteryBars = buffer[1];

    if (data.batteryBars > 16)
    {
        data.batteryBars = 16;
    }

    data.power = buffer[8] * 13;
    data.temperature = int8_t(buffer[9]) + 15;

    bool brake = (buffer[7] & 32) == 32;

    if (brake)
    {
        data.gearState |= BRAKE;
    }
    else
    {
        data.gearState &= ~BRAKE;
    }
}

bool Controller::isDataValid(byte buffer[], int counter)
{
    int checksum = calculateChecksum(buffer);

    if (buffer[0] == 65 && buffer[6] == checksum)
    {
        return true;
    }

    if (counter >= 3)
    {
        return false;
    }

    byte newBuffer[BUFFER_SIZE];
    int shift = 0;

    for (int i = 1; i < BUFFER_SIZE; i++)
    {
        if (buffer[i] == 65)
        {
            shift = i;
        }
    }

    shift = BUFFER_SIZE - shift;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (i < shift)
        {
            newBuffer[i] = buffer[BUFFER_SIZE + i - shift];
        }
        else
        {
            newBuffer[i] = buffer[i - shift];
        }
    }

    memcpy(buffer, newBuffer, sizeof(newBuffer));

    return isDataValid(buffer, counter + 1);
}

int Controller::calculateChecksum(byte buffer[])
{
    int crc = 0;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (i != 6 && i != 0)
        {
            crc ^= buffer[i];
        }
    }
    return crc;
}