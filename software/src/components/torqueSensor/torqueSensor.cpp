#include "./torqueSensor.hpp"
/**
 * Initialize the torque sensor and fill the torque table with 0
 * @return void
 */
TorqueSensor::TorqueSensor() { populateTorqueArray(); }

/**
 * Main loop for the torque sensor
 * @return void
 */
void TorqueSensor::handleTorqueSensor() {
  currentTorque = analogRead(TORQUE_INPUT_PIN);
  if (currentTorque > 0) {
    shiftTorqueArray(currentTorque);
  }
  int writeTorque =
      settings.enableTorqueSensor ? calculateTorqueOutput(torqueArrayMax()) : 0;
  dacWrite(TORQUE_OUTPUT_PIN, writeTorque);
}

/**
 * Populate the torque array with 0
 * @return void
 */
void TorqueSensor::populateTorqueArray() {
  for (int i = 0; i < TORQUE_ARRAY_SIZE; i++) {
    torqueArray[i] = 0;
  }
}

/**
 * Shift the torque array to the left and add the new torque value
 * @param value the new torque value
 * @return void
 */
void TorqueSensor::shiftTorqueArray(int value) {
  for (int i = 0; i < TORQUE_ARRAY_SIZE - 1; i++) {
    torqueArray[i] = torqueArray[i + 1];
  }
  torqueArray[TORQUE_ARRAY_SIZE - 1] = value;
}

/**
 * Get the maximum torque value from the torque array over the last T1 ms
 * @return int the maximum torque value
 */
int TorqueSensor::torqueArrayMax() {
  int max = 0;
  for (int i = TORQUE_ARRAY_SIZE - (settings.torqueSensorCutOff);
       i < TORQUE_ARRAY_SIZE; i++) {
    if (torqueArray[i] > max) {
      max = torqueArray[i];
    }
  }
  return max;
}

/**
 * Calculate the torque output for the DAC based on the maximum torque value and
 * current gear
 * @param torque the torque value
 * @return int the torque output
 */
int TorqueSensor::calculateTorqueOutput(int torque) {
  int writeTorque =
      map(torque, 0, 4096, 0, 3301); // map adc readout to voltage (mV)
  writeTorque -= TORQUE_OFFSET;      // subtract offset
  if (writeTorque < 0 || settings.currentGear == 0) {
    return 0;
  } else if (writeTorque > 1500) {
    writeTorque = 1500;
  }
  return map(writeTorque, 0, 1501, TORQUE_OUTPUT_MIN,
             torqueOutputTable[settings.currentGear]);
}