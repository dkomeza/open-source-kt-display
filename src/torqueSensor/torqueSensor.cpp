#include "./torqueSensor.h"

#include "../logic/logic.h"

TorqueSensor::TorqueSensor() {
  populateTorqueArray();
}

void TorqueSensor::handleTorqueSensor() {
  currentTorque = analogRead(TORQUE_INPUT_PIN);
  if (currentTorque > 0) {
    shiftTorqueArray(currentTorque);
  }
  int writeTorque = settings.enableTorqueSensor ? calculateTorqueOutput(torqueArrayMax()) : 0;
  dacWrite(TORQUE_OUTPUT_PIN, writeTorque);
}

void TorqueSensor::populateTorqueArray() {
  for (int i = 0; i < TORQUE_ARRAY_SIZE; i++) {
    torqueArray[i] = 0;
  }
}
// add a new value to the torque array and shift the rest of the values
void TorqueSensor::shiftTorqueArray(int value) {
  for (int i = 0; i < TORQUE_ARRAY_SIZE - 1; i++) {
    torqueArray[i] = torqueArray[i + 1];
  }
  torqueArray[TORQUE_ARRAY_SIZE - 1] = value;
}
// get the maximum value from the last T1/50 values
int TorqueSensor::torqueArrayMax() {
  int max = 0;
  for (int i = TORQUE_ARRAY_SIZE - (settings.torqueSensorCutOff); i < TORQUE_ARRAY_SIZE; i++) {  // Select the last T1/50 values (T1 - user defined, 50 - 50ms loop time)
    if (torqueArray[i] > max) {
      max = torqueArray[i];
    }
  }
  return max;
}
// map the max torque value to the torque output range
int TorqueSensor::calculateTorqueOutput(int torque) {
  int writeTorque = map(torque, 0, 4096, 0, 3301);  // map adc readout to voltage (mV)
  writeTorque -= TORQUE_OFFSET;                     // subtract offset
  if (writeTorque < 0 || settings.currentGear == 0) {
    return 0;
  } else if (writeTorque > 1500) {
    writeTorque = 1500;
  }
  return map(writeTorque, 0, 1501, TORQUE_OUTPUT_MIN, torqueOutputTable[settings.currentGear]);
}