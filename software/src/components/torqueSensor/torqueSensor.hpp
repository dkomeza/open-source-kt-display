#ifndef TORQUESENSOR_H
#define TORQUESENSOR_H

#define TORQUE_ARRAY_SIZE 40
#define TORQUE_OUTPUT_TABLE_SIZE 5

#define TORQUE_INPUT_PIN 32
#define TORQUE_OUTPUT_PIN 25

#define TORQUE_OFFSET 1500
#define TORQUE_OUTPUT_MIN 39

#include <Arduino.h>

#include "../settings/settings.hpp"

class TorqueSensor {
 private:
  const int torqueOutputTable[TORQUE_OUTPUT_TABLE_SIZE] = {55, 70, 90, 125, 163};
  int torqueArray[TORQUE_ARRAY_SIZE];
  int currentTorque = 0;
  int torqueVoltage = 0;

 public:
  TorqueSensor();
  void handleTorqueSensor();

 private:
  void populateTorqueArray();
  void shiftTorqueArray(int value);
  int torqueArrayMax();
  int calculateTorqueOutput(int torque);
};

extern TorqueSensor torqueSensor;

#endif