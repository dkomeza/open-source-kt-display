#ifndef OSKD_CONTROLLER_H
#define OSKD_CONTROLLER_H

#include <Arduino.h>
#include <HardwareSerial.h>

#include "../data/data.h"
#include "../screen/screen.h"

class Controller
{
public:
    void setup();

    void update();

private:
    HardwareSerial controllerSerial = HardwareSerial(2);

    const int controllerBaudRate = 9600;
    const int controllerRXPin = 16;
    const int controllerTXPin = 17;

    const int BUFFER_UP_SIZE = 13;
    const int BUFFER_SIZE = 12;

    const double RPM_CONSTANT = 0.1885;

    void sendData();
    void receiveData();

    void parseData(byte buffer[]);
    bool isDataValid(byte buffer[], int counter);

    int calculateChecksum(byte buffer[]);
};

extern Controller controller;

#endif // OSKD_CONTROLLER_H