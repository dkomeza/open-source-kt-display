#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>

#include "../display/display.h"

#define MENU_SIZE 17
#define BUFFER_SIZE 12
#define BUFFER_SIZE_UP 13

#define SPEED_LIMIT values[0]
#define WHEEL_SIZE values[1]
#define P1 values[2]
#define P2 values[3]
#define P3 values[4]
#define P4 values[5]
#define P5 values[6]
#define C1 values[7]
#define C2 values[8]
#define C4 values[9]
#define C5 values[10]
#define C11 values[11]
#define C12 values[12]
#define C13 values[13]
#define C14 values[14]
#define T1 values[15]

class Settings {
 private:
  const String names[MENU_SIZE] = {"Speed limit", "Wheel size", "P1", "P2", "P3", "P4", "P5", "C1", "C2", "C4", "C5", "C11", "C12", "C13", "C14", "T1", "T2"};
  int values[MENU_SIZE];
  const int minValues[MENU_SIZE] = {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -100};
  const int maxValues[MENU_SIZE] = {72, 14, 255, 6, 1, 1, 30, 7, 7, 4, 10, 3, 7, 5, 3, 40, 100};
  const int defaultValues[MENU_SIZE] = {72, 12, 86, 1, 1, 0, 13, 5, 0, 0, 10, 0, 4, 0, 1, 10, 0};
  const int wheelSizeTable[15][2] = {{50, 22}, {60, 18}, {80, 10}, {100, 14}, {120, 2}, {140, 6}, {160, 0}, {180, 4}, {200, 8}, {230, 12}, {240, 16}, {260, 20}, {275, 24}, {280, 28}, {290, 30}};  // {wheel size * 10, byte value}

  int cursorPosition[2] = {102, 80};
  int previousCursorPosition[2] = {0, 0};

  void calculateCursorPosition();
  int calculateUpCRC(byte packet[]);

 public:
  bool settingsMenu = false;
  int cursorPositionCounter = 0;
  bool selectedOption = false;
  byte settings[BUFFER_SIZE_UP];

  double batteryVoltageOffset = 0;

  int speedLimit = 0;
  bool limitState = false;

  bool enableTorqueSensor = false;

  int currentGear = 0;
  int walkGear = 0;
  int gearColor = 0;
  int maxGear = 5;

  int torqueSensorCutOff = values[15];

  void renderSettingsMenu();
  void updateCursorPosition();
  void toggleOption();
  void changeSetting(int direction);
  void calculatePacket();
  bool checkInitialSettings();
  void loadSettings();
  void saveSettings();
  void handleLimit();
};

extern Settings settings;

#endif