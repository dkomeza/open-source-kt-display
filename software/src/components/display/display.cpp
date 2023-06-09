#include "display.hpp"

/**
 * Initialize the display
 * @return void
 */
void Display::init() {
  tft.init();
  tft.setRotation(6);
  initialRender();
}

void Display::renderIP(IPAddress IP) {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(40, 40);
  tft.print(IP);
}

/**
 * Render the initial display
 * @return void
 */
void Display::initialRender() {
  tft.fillScreen(TFT_BLACK);
  tft.drawRoundRect(10, 16, 80, 24, 4, TFT_GREEN);
  tft.fillRect(90, 23, 3, 10, TFT_GREEN);
  tft.fillRoundRect(92, 23, 3, 10, 2, TFT_GREEN);

  tft.drawFastHLine(0, 54, 240, TFT_WHITE);
  tft.drawFastHLine(0, 248, 240, TFT_WHITE);
}

void Display::clearScreen() {
  tft.fillScreen(TFT_GOLD);
}

/**
 * Render the display
 * @param batteryLevel the battery level to draw (0-4)
 * @param batteryVoltage the battery voltage to draw
 * @param speed the speed to draw * 10
 * @param engineTemp the engine temperature to draw
 * @param controllerTemp the controller temperature to draw
 * @param power the power to draw
 */
void Display::render(int batteryLevel, int batteryVoltage, int speed, int engineTemp, int controllerTemp, int power) {
  tft.setTextFont(4);
  updateBattery(batteryLevel, batteryVoltage);
  updateSpeed(speed);
  updateEngineTemp(engineTemp);
  updatePower(power);
}

/**
 * Update the gear on the display
 * @param currentGear the gear to draw
 * @param gearColor the color of the gear to draw (0: yellow (normal), 1: green ("legal mode"), 2: red (braking))
 * @return void
 */
void Display::updateGear(int currentGear, int gearColor) {
  tft.setTextFont(7);
  tft.setTextSize(1);
  if (gearColor == 0) {
    tft.setTextColor(TFT_GREEN, 0);
  } else if (gearColor == 1) {
    tft.setTextColor(TFT_YELLOW, 0);
  } else {
    tft.setTextColor(TFT_RED, 0);
  }
  tft.setCursor(108, 180);
  tft.print(currentGear);
}

/**
 * Update the torque icon on the display
 * @param enableTorqueSensor whether or not to show the torque icon
 * @return void
 */
void Display::updateTorqueIcon(bool enableTorqueSensor) {
  if (enableTorqueSensor) {
    tft.fillCircle(88, 180, 8, TFT_GREEN);
  } else {
    tft.fillCircle(88, 180, 8, TFT_BLACK);
  }
}

/**
 * Render the settings menu
 * @param menuSize the size of the menu
 * @param names the names of the settings
 * @param wheelSizeTable the wheel size table
 * @param values the values of the settings
 * @return void
 */
void Display::renderSettingsMenu(int menuSize, const String names[], const int wheelSizeTable[][2], int values[]) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(4);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, 0);
  tft.setCursor(16, 16);
  tft.print("settings");
  tft.drawFastHLine(0, 54, 240, TFT_WHITE);
  tft.drawFastHLine(0, 55, 240, TFT_WHITE);
  tft.drawFastHLine(0, 56, 240, TFT_WHITE);
  tft.setTextFont(2);
  tft.drawFastVLine(132, 55, 207, TFT_WHITE);
  for (int i = 0; i < menuSize; i++) {
    if (i < 9) {
      tft.setCursor(8, 66 + (i * 22));
      tft.drawFastHLine(0, 63 + ((i + 1) * 22), 132, TFT_WHITE);
    } else {
      tft.setCursor(138, 66 + ((i - 9) * 22));
      tft.drawFastHLine(132, 63 + ((i - 8) * 22), 108, TFT_WHITE);
    }
    tft.print(names[i]);
    tft.print(": ");
    if (i == 1) {
      int a = wheelSizeTable[values[i]][0] / 10;
      int b = wheelSizeTable[values[i]][0] - a * 10;

      if (b > 0) {
        tft.print(a);
        tft.print(".");
        tft.print(b);
      } else {
        tft.print(a);
        tft.print("  ");
      }
    } else if (i == 15) {
      tft.print(values[i] * 50);
    } else {
      tft.print(values[i]);
    }
  }
  tft.drawFastHLine(0, 261, 240, TFT_WHITE);
  tft.drawFastHLine(0, 262, 240, TFT_WHITE);
  tft.drawFastHLine(0, 263, 240, TFT_WHITE);
  tft.setCursor(8, 266);
  tft.setTextFont(4);
  tft.print("Packet");
  tft.setTextFont(2);
}

/**
 * Update the cursor position on the settings menu
 * @param cursorPosition the cursor position to draw
 * @param previousCursorPosition the previous cursor position to erase
 * @return void
 */
void Display::updateCursorPosition(int previousCursorPosition[2], int cursorPosition[2]) {
  tft.fillTriangle(previousCursorPosition[0], previousCursorPosition[1], previousCursorPosition[0] + 8, previousCursorPosition[1] + 6, previousCursorPosition[0] + 8, previousCursorPosition[1] - 6, TFT_BLACK);
  tft.fillTriangle(cursorPosition[0], cursorPosition[1], cursorPosition[0] + 8, cursorPosition[1] + 6, cursorPosition[0] + 8, cursorPosition[1] - 6, TFT_WHITE);
  previousCursorPosition[0] = cursorPosition[0];
  previousCursorPosition[1] = cursorPosition[1];
}

/**
 * Update the cursor position on the settings menu
 * @param cursorPositionCounter the cursor position to draw
 * @param name the name of the setting
 * @param value the value of the setting
 * @param wheelSize the wheel size
 * @param active whether or not the setting is active
 * @return void
 */
void Display::printOption(int cursorPositionCounter, String name, int value, int wheelSize, bool active) {
  if (cursorPositionCounter < 9) {
    tft.setCursor(8, 66 + (cursorPositionCounter * 22));
  } else {
    tft.setCursor(138, 66 + ((cursorPositionCounter - 9) * 22));
  }
  tft.print(name);
  tft.print(": ");
  if (active) {
    tft.setTextColor(TFT_YELLOW, 0);
  } else {
    tft.setTextColor(TFT_WHITE, 0);
  }
  if (cursorPositionCounter == 1) {
    int a = wheelSize / 10;
    int b = wheelSize - a * 10;
    tft.print(a);
    if (b > 0) {
      tft.print(".");
      tft.print(b);
    } else {
      tft.print("  ");
    }
  } else if (cursorPositionCounter == 15) {
    tft.print(value * 50);
  } else {
    tft.print(value);
  }
  tft.setTextColor(TFT_WHITE, 0);
}

/**
 * Update the battery level on the display (battery bars and voltage)
 * @param bars the number of battery bars to draw
 * @param batteryVoltage the battery voltage to draw
 * @return void
 */
void Display::updateBattery(int bars, int batteryVoltage) {
  int _bars = bars / 4;
  if (_bars > 4) {
    _bars = 4;
  }
  if (_bars < 0) {
    _bars = 0;
  }
  if (previousBatteryLevel != _bars) {
    previousBatteryLevel = _bars;
    tft.fillRect(10, 16, 100, 25, TFT_BLACK);
    tft.drawRoundRect(10, 16, 80, 24, 4, TFT_GREEN);
    tft.fillRect(90, 23, 3, 10, TFT_GREEN);
    tft.fillRoundRect(92, 23, 3, 10, 2, TFT_GREEN);
    for (int i = 0; i < _bars; i++) {
      int x = i * 19 + 13;
      tft.fillRoundRect(x, 19, 17, 18, 2, TFT_GREEN);
    }
  }
  tft.setCursor(164, 16);
  tft.setTextColor(TFT_GREEN, 0);
  int intPart = batteryVoltage / 10;
  int floatPart = batteryVoltage - (intPart * 10);
  tft.setTextSize(1);
  tft.print(intPart);
  tft.print(".");
  tft.print(floatPart);
  tft.print("V");
}

/**
 * Update the speed on the display
 * @param speed the speed to draw * 10
 * @return void
 */
void Display::updateSpeed(int speed) {
  if (speed < 1000) {  // make sure the speed is always 3 digits or less
    tft.setTextColor(TFT_WHITE, 0);
    tft.setCursor(40, 86);
    tft.setTextFont(8);
    tft.setTextSize(1);
    int a = 0;
    int b = 0;
    if (speed > 10) {
      a = speed / 10;
      b = speed - (a * 10);
    }
    if (a < 10) {
      tft.print(0);
      tft.print(a);
    } else {
      tft.print(a);
    }
    tft.setTextFont(6);
    tft.setTextSize(1);
    tft.setCursor(145, 124);
    tft.print(".");
    tft.print(b);
  }
}

/**
 * Update the engine temperature on the display
 * @param engineTemp the engine temperature to draw
 * @return void
 */
void Display::updateEngineTemp(int engineTemp) {
  tft.setTextFont(0);
  tft.setTextSize(2);
  tft.setCursor(16, 265);
  tft.setTextColor(TFT_WHITE, 0);
  tft.print("MTP:");
  tft.setTextColor(TFT_YELLOW, 0);
  if (engineTemp < -10) {
    tft.print(" ");
  } else if (engineTemp < 0) {
    tft.print("  ");
  } else if (engineTemp < 10) {
    tft.print("   ");
  } else if (engineTemp < 100) {
    tft.print("  ");
  } else {
    tft.print(" ");
  }
  tft.print(engineTemp);
  tft.print("C");
}

/**
 * Update the motor power on the display
 * @param power the motor power (watts)
 * @return void
 */
void Display::updatePower(int power) {
  tft.setTextFont(0);
  tft.setTextSize(2);
  tft.setCursor(16, 290);
  tft.setTextColor(TFT_WHITE, 0);
  tft.print("MP:");
  tft.setTextColor(TFT_YELLOW, 0);
  if (power < 10) {
    tft.print("   ");
  } else if (power < 100) {
    tft.print("  ");
  } else if (power < 1000) {
    tft.print(" ");
  }
  tft.print(power);
  tft.println("W");
}