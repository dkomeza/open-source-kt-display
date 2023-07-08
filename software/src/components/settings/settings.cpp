#include "settings.hpp"

#define EEPROM_SIZE 512

/**
 * Initialize the settings menu
 * @return void
 */
void Settings::renderSettingsMenu() {
  display.renderSettingsMenu(MENU_SIZE, names, wheelSizeTable, values);
  updateCursorPosition();
}

/**
 * Update the cursor position on the display
 * @return void
 */
void Settings::updateCursorPosition() {
  previousCursorPosition[0] = cursorPosition[0];
  previousCursorPosition[1] = cursorPosition[1];
  calculateCursorPosition();
  display.updateCursorPosition(previousCursorPosition, cursorPosition);
}

/**
 * Toggle the selected state for the current setting
 * @return void
 */
void Settings::toggleOption() {
  selectedOption = !selectedOption;
  display.printOption(cursorPositionCounter, names[cursorPositionCounter],
                      values[cursorPositionCounter],
                      wheelSizeTable[values[cursorPositionCounter]][0],
                      selectedOption);
}

/**
 * Increment the current setting
 * @param direction the direction to increment
 * @return void
 */
void Settings::changeSetting(int direction) {
  if (values[cursorPositionCounter] < maxValues[cursorPositionCounter] &&
      direction == 1) {
    values[cursorPositionCounter] += direction;
  }
  if (values[cursorPositionCounter] > minValues[cursorPositionCounter] &&
      direction == -1) {
    values[cursorPositionCounter] += direction;
  }
  display.printOption(cursorPositionCounter, names[cursorPositionCounter],
                      values[cursorPositionCounter],
                      wheelSizeTable[values[cursorPositionCounter]][0],
                      selectedOption);
}

/**
 * Calculate the cursor position
 * @return void
 */
void Settings::calculateCursorPosition() {
  if (cursorPositionCounter < 9) {
    cursorPosition[0] = 116;
    cursorPosition[1] = cursorPositionCounter * 22 + 74;
  } else {
    cursorPosition[0] = 228;
    cursorPosition[1] = (cursorPositionCounter - 9) * 22 + 74;
  }
}

/**
 * Check if the settings loaded from EEPROM are valid
 * @return bool - true if the settings are valid, false if not
 */
bool Settings::checkInitialSettings() {
  for (int i = 0; i < MENU_SIZE; i++) {
    if (EEPROM.read(i) > maxValues[i] || EEPROM.read(i) < minValues[i]) {
      return false;
    }
  }
  return true;
}

/**
 * Load the settings from EEPROM
 * @return void
 */
void Settings::loadSettings() {
  if (checkInitialSettings()) {
    for (int i = 0; i < MENU_SIZE; i++) {
      values[i] = EEPROM.read(i);
    }
  } else {
    for (int i = 0; i < MENU_SIZE; i++) {
      values[i] = defaultValues[i];
    }
  }
};

/**
 * Save the settings to EEPROM
 * @return void
 */
void Settings::saveSettings() {
  for (int i = 0; i < MENU_SIZE; i++) {
    EEPROM.write(i, values[i]);
  }
  EEPROM.commit();
}

/**
 * Calculate the packet to send to the controller
 * @return void
 */
void Settings::calculatePacket() {
  int speed = 0;
  if (speedLimit > 0) {
    speed = speedLimit;
  } else {
    speed = SPEED_LIMIT;
  }
  int P_4 = enableTorqueSensor ? 0 : P4;
  int C_4 = enableTorqueSensor ? 4 : C4;
  settings[0] = P5;
  settings[1] = enableTorqueSensor ? 0 : currentGear;
  settings[2] =
      (((speed - 10) & 31) << 3) | (wheelSizeTable[WHEEL_SIZE][1] >> 2);
  settings[3] = P1;
  settings[4] = ((wheelSizeTable[WHEEL_SIZE][1] & 3) << 6) |
                ((speed - 10) & 32) | (P_4 << 4) | P3 << 3 | P2;
  settings[5] = 0;
  settings[6] = (C1 << 3) | C2;
  settings[7] = (C14 << 5) | C5 | 128;
  settings[8] = (C_4 << 5) | C12;
  settings[9] = 20;
  settings[10] = C13 << 2 | 1;
  settings[11] = 50;
  settings[12] = 14;
  settings[5] = calculateUpCRC(settings);
}

/**
 * Handle the speed limit setting
 * @return void
 */
void Settings::handleLimit() {
  if (limitState) {
    gearColor = 0;
    if (currentGear > 2) {
      currentGear = 2;
    }
    maxGear = 2;
    speedLimit = 25;
    calculatePacket();
    display.updateGear(currentGear, gearColor);
  } else {
    gearColor = 1;
    maxGear = 5;
    speedLimit = 0;
    calculatePacket();
    display.updateGear(currentGear, gearColor);
  }
}

/**
 * Calculate the CRC for the packet
 * @param packet the packet to calculate the CRC for
 * @return int - the calculated CRC
 */
int Settings::calculateUpCRC(byte packet[]) {
  int crc = 0;
  for (int i = 0; i < BUFFER_SIZE_UP; i++) {
    if (i != 5) {
      crc ^= packet[i];
    }
  }
  crc ^= 3;
  return crc;
}