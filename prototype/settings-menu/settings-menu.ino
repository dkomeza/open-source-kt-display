#include <EEPROM.h>
#include <HardwareSerial.h>
#include <OneButton.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// EEPROM defines
#define EEPROM_SIZE 512
#define MENU_SIZE 15

OneButton buttonUp(14, true);
OneButton buttonDown(13, true);
OneButton buttonPower(12, true);

// initialize display
TFT_eSPI tft = TFT_eSPI(240, 320);

// initialize serial port
HardwareSerial SerialPort(2);

int cursorPositionCounter = 0;
int cursorPosition[2] = {102, 80};
int previousCursorPosition[2] = {0, 0};

String NAMES[MENU_SIZE] = {"Speed limit", "Wheel size", "P1", "P2", "P3", "P4", "P5", "C1", "C2", "C4", "C5", "C11", "C12", "C13", "C14"};
int VALUES[MENU_SIZE] = {72, 12, 86, 1, 1, 0, 13, 5, 0, 0, 10, 0, 4, 0, 1};
const int MIN_VALUES[MENU_SIZE] = {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
const int MAX_VALUES[MENU_SIZE] = {72, 14, 14, 6, 1, 1, 30, 7, 1, 4, 10, 3, 7, 5, 3};
const int WHEEL_SIZE_TABLE[15][2] = {{50, 22}, {60, 18}, {80, 10}, {100, 14}, {120, 2}, {140, 6}, {160, 0}, {180, 4}, {200, 8}, {230, 12}, {240, 16}, {260, 20}, {275, 24}, {280, 28}, {290, 30}};

int speedLimit = VALUES[0];

// initialize byte arrays for serial communication
const int BUFFER_SIZE = 12;
const int BUFFER_SIZE_UP = 13;

byte SETTINGS[BUFFER_SIZE_UP];
byte buf_up[BUFFER_SIZE_UP];

bool settingsMenu = false;
bool selectedOption = false;

void setup() {
  // setup display
  tft.init();
  tft.setRotation(6);
  initialRender();

  // initialize eeprom
  EEPROM.begin(EEPROM_SIZE);

  // attach button functions
  buttonUp.attachClick(handleUpButtonClick);
  buttonUp.attachLongPressStart(handleUpButtonLongPressStart);
  buttonDown.attachClick(handleDownButtonClick);
  buttonDown.attachLongPressStart(handleDownButtonLongPressStart);
  buttonDown.attachLongPressStop(handleDownButtonLongPressStop);
  buttonPower.attachClick(handlePowerButtonClick);
  buttonPower.attachLongPressStart(handlePowerButtonLongPressStart);

  getDataFromEEPROM();
  calculatePacket();
  saveToLocal();

  // setup serial ports
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
}

void loop() {
  if (settingsMenu) {
    updateCursor(false);
  }
  buttonUp.tick();
  buttonDown.tick();
  buttonPower.tick();
  delay(10);
}

void handleUpButtonClick() {
  if (settingsMenu) {
    if (cursorPositionCounter > 0 && !selectedOption) {
      cursorPositionCounter--;
      calculateCursorPosition();
    } else if (selectedOption) {
      handleChange(cursorPositionCounter, "UP");
    }
  }
}

void handleDownButtonClick() {
  if (settingsMenu) {
    if (cursorPositionCounter < 14 && !selectedOption) {
      cursorPositionCounter++;
      calculateCursorPosition();
    } else if (selectedOption) {
      handleChange(cursorPositionCounter, "DOWN");
    }
  }
}

void handleUpButtonLongPressStart() {
  Serial.println("Up button long press start");
}

void handleDownButtonLongPressStart() {
  Serial.println("Down button long press start");
}

void handleDownButtonLongPressStop() {
  Serial.println("Down button long press stop");
}

void handlePowerButtonClick() {
  if (settingsMenu) {
    if (selectedOption) {
      deselectOption(cursorPositionCounter);
    } else {
      selectOption(cursorPositionCounter);
    }
  }
}

void handlePowerButtonLongPressStart() {
  if (!settingsMenu) {
    renderSettingsMenu();
    settingsMenu = !settingsMenu;
  } else {
    if (!selectedOption) {
      initialRender();
      settingsMenu = !settingsMenu;
      cursorPositionCounter = 0;
      saveDataToEEPROM();
      saveToLocal();
    }
  }
}

void initialRender() {
  tft.fillScreen(TFT_BLACK);
  // Draw battery
  tft.drawRoundRect(10, 16, 80, 24, 4, TFT_GREEN);
  tft.fillRect(90, 23, 3, 10, TFT_GREEN);
  tft.fillRoundRect(92, 23, 3, 10, 2, TFT_GREEN);

  tft.drawFastHLine(0, 54, 240, TFT_WHITE);
  tft.drawFastHLine(0, 248, 240, TFT_WHITE);
}

void renderSettingsMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(4);
  tft.setCursor(16, 16);
  tft.print("Settings");
  tft.drawFastHLine(0, 54, 240, TFT_WHITE);
  tft.drawFastHLine(0, 55, 240, TFT_WHITE);
  tft.drawFastHLine(0, 56, 240, TFT_WHITE);
  tft.setTextFont(2);
  tft.drawFastVLine(132, 55, 207, TFT_WHITE);
  for (int i = 0; i < MENU_SIZE; i++) {
    if (i < 9) {
      tft.setCursor(8, 66 + (i * 22));
      tft.drawFastHLine(0, 63 + ((i + 1) * 22), 132, TFT_WHITE);
    } else {
      tft.setCursor(138, 66 + ((i - 9) * 22));
      tft.drawFastHLine(132, 63 + ((i - 8) * 22), 108, TFT_WHITE);
    }
    tft.print(NAMES[i]);
    tft.print(": ");
    if (i == 1) {
      int a = WHEEL_SIZE_TABLE[VALUES[i]][0] / 10;
      int b = WHEEL_SIZE_TABLE[VALUES[i]][0] - a * 10;

      if (b > 0) {
        tft.print(a);
        tft.print(".");
        tft.print(b);
      } else {
        tft.print(a);
        tft.print("  ");
      }
    } else {
      tft.print(VALUES[i]);
    }
  }
  tft.drawFastHLine(0, 261, 240, TFT_WHITE);
  tft.drawFastHLine(0, 262, 240, TFT_WHITE);
  tft.drawFastHLine(0, 263, 240, TFT_WHITE);
  tft.setCursor(8, 266);
  tft.setTextFont(4);
  tft.print("Packet");
  tft.setTextFont(2);
  calculateCursorPosition();
  calculatePacket();
  drawPacket();
  updateCursor(true);
}

void calculateCursorPosition() {
  if (cursorPositionCounter < 9) {
    cursorPosition[0] = 116;
    cursorPosition[1] = cursorPositionCounter * 22 + 74;
  } else {
    cursorPosition[0] = 202;
    cursorPosition[1] = (cursorPositionCounter - 9) * 22 + 74;
  }
}

void updateCursor(bool force) {
  if (cursorPosition[0] != previousCursorPosition[0] || cursorPosition[1] != previousCursorPosition[1] || force) {
    tft.fillTriangle(previousCursorPosition[0], previousCursorPosition[1], previousCursorPosition[0] + 8, previousCursorPosition[1] + 6, previousCursorPosition[0] + 8, previousCursorPosition[1] - 6, TFT_BLACK);
    tft.fillTriangle(cursorPosition[0], cursorPosition[1], cursorPosition[0] + 8, cursorPosition[1] + 6, cursorPosition[0] + 8, cursorPosition[1] - 6, TFT_WHITE);
    previousCursorPosition[0] = cursorPosition[0];
    previousCursorPosition[1] = cursorPosition[1];
  }
}

void selectOption(int position) {
  selectedOption = true;
  if (position < 9) {
    tft.setCursor(8, 66 + (position * 22));
  } else {
    tft.setCursor(138, 66 + ((position - 9) * 22));
  }
  tft.print(NAMES[position]);
  tft.print(": ");
  tft.setTextColor(TFT_YELLOW, 0);
  if (position == 1) {
    int a = WHEEL_SIZE_TABLE[VALUES[position]][0] / 10;
    int b = WHEEL_SIZE_TABLE[VALUES[position]][0] - a * 10;
    tft.print(a);
    if (b > 0) {
      tft.print(".");
      tft.print(b);
    } else {
      tft.print("  ");
    }
  } else {
    tft.print(VALUES[position]);
  }
  tft.setTextColor(TFT_WHITE, 0);
}

void deselectOption(int position) {
  selectedOption = false;
  if (position < 9) {
    tft.setCursor(8, 66 + (position * 22));
  } else {
    tft.setCursor(138, 66 + ((position - 9) * 22));
  }
  tft.print(NAMES[position]);
  tft.print(": ");
  if (position == 1) {
    int a = WHEEL_SIZE_TABLE[VALUES[position]][0] / 10;
    int b = WHEEL_SIZE_TABLE[VALUES[position]][0] - a * 10;
    tft.print(a);
    if (b > 0) {
      tft.print(".");
      tft.print(b);
    } else {
      tft.print("  ");
    }
  } else {
    tft.print(VALUES[position]);
  }
  calculatePacket();
  drawPacket();
}

void handleChange(int position, String direction) {
  if (direction == "UP") {
    if (VALUES[position] < MAX_VALUES[position]) {
      VALUES[position]++;
    }
  } else if (direction == "DOWN") {
    if (VALUES[position] > MIN_VALUES[position]) {
      VALUES[position]--;
    }
  }
  if (position < 9) {
    tft.setCursor(8, 66 + (position * 22));
  } else {
    tft.setCursor(138, 66 + ((position - 9) * 22));
  }
  tft.print(NAMES[position]);
  tft.print(": ");
  tft.setTextColor(TFT_YELLOW, 0);
  if (position == 1) {
    int a = WHEEL_SIZE_TABLE[VALUES[position]][0] / 10;
    int b = WHEEL_SIZE_TABLE[VALUES[position]][0] - a * 10;
    tft.print(a);
    if (b > 0) {
      tft.print(".");
      tft.print(b);
    } else {
      tft.print("  ");
    }
  } else {
    tft.print(VALUES[position]);
  }
  tft.setTextColor(TFT_WHITE, 0);
}

void calculatePacket() {
  SETTINGS[0] = VALUES[6];
  SETTINGS[1] = 0;
  SETTINGS[2] = (((speedLimit - 10) & 31) << 3) | (WHEEL_SIZE_TABLE[VALUES[1]][1] >> 2);
  SETTINGS[3] = VALUES[2];
  SETTINGS[4] = ((WHEEL_SIZE_TABLE[VALUES[1]][1] & 3) << 6) | ((speedLimit - 10) & 32) | (VALUES[5] << 4) | VALUES[4] << 3 | VALUES[3];
  SETTINGS[5] = 0;
  SETTINGS[6] = (VALUES[7] << 3) | VALUES[8];
  SETTINGS[7] = (VALUES[14] << 5) | VALUES[10] | 128;
  SETTINGS[8] = (VALUES[9] << 5) | VALUES[12];
  SETTINGS[9] = 20;
  SETTINGS[10] = VALUES[13] << 2 | 1;
  SETTINGS[11] = 50;
  SETTINGS[12] = 14;
  SETTINGS[5] = calculateUpCRC();
}

void drawPacket() {
  tft.setCursor(8, 280);
  for (int i = 0; i < BUFFER_SIZE_UP; i++) {
    tft.print(buf_up[i]);
    tft.print(", ");
  }
}

int calculateUpCRC() {
  int crc = 0;
  for (int i = 0; i < BUFFER_SIZE_UP; i++) {
    if (i != 5) {
      crc ^= SETTINGS[i];
    }
  }
  crc ^= 3;
  return crc;
}

void getDataFromEEPROM() {
  for (int i = 0; i < 15; i++) {
    VALUES[i] = EEPROM.read(i);
  }
}

void saveDataToEEPROM() {
  for (int i = 0; i < 15; i++) {
    EEPROM.write(i, VALUES[i]);
  }
  EEPROM.commit();
}

void saveToLocal() {
  for (int i = 0; i < 15; i++) {
    buf_up[i] = SETTINGS[i];
  }
}
