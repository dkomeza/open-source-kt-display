#ifndef LOGIC_H
#define LOGIC_H

#include <Arduino.h>

#include "../settings/settings.hpp"

#define BUFFER_SIZE 12
#define BATTERY_INPUT_PIN 33

class Logic {
 private:
  int calculateDownCRC();
  bool shiftArray(int counter);
  double getBatteryVoltage();

 public:
  int speed = 0;
  int batteryLevel = 0;
  int batteryVoltage = 0;
  int power = 0;
  int engineTemp = 0;
  bool braking = false;
  byte buf[BUFFER_SIZE];

  bool processPacket();
};

extern Logic logic;

#endif