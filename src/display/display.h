#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

class Display {
 public:
  Display() {
  }
  void init();
  void initialRender();
  void render(int batteryLevel, int batteryVoltage, int speed, int engineTemp, int controllerTemp, int power);
  void updateGear(int currentGear, int gearColor);
  void renderSettingsMenu(int menuSize, const String names[], const int wheelSizeTable[][2], int values[]);
  void updateCursorPosition(int previousCursorPosition[2], int cursorPosition[2]);
  void printOption(int cursorPositionCounter, String name, int value, int wheelSize, bool active);
  void updateTorqueIcon(bool enableTorqueSensor);

 private:
  TFT_eSPI tft = TFT_eSPI(240, 320);
  void updateBattery(int batteryLevel, int batteryVoltage);
  void updateSpeed(int speed);
  void updateEngineTemp(int engineTemp);
  void updatePower(int power);
};

#endif